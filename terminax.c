/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pty.h>
#include <termios.h>
#include <sys/select.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#define W 800
#define H 600
#define M 1000
#define F 8
#define T 16

Display *d; Window w; GC g; int p;

typedef struct {
        char *t;
} L;

L l[M]; int c = 0; int s = 0; int x = W; int y = H; int z = W / F; int r = H / T;

void f() {
        for (int i = 0; i < c; i++) {
                free(l[i].t);
                l[i].t = NULL;
        }
        c = 0;
}

void a(const char *v) {
        if (c == M) {
                free(l[0].t);
                memmove(l, l + 1, sizeof(L) * (M - 1));
                c--;
        }
        l[c].t = strdup(v);
        if (!l[c].t) {
                perror("strdup");
                exit(1);
        }
        c++;
}

void w_a(const char *v) {
        int n = strlen(v);
        int k = 0;
        while (k < n) {
                int m = (n - k > z) ? z : (n - k);
                char b[z + 1];
                memcpy(b, v + k, m);
                b[m] = '\0';
                a(b);
                k += m;
        }
}

void p_o(const char *b, int n) {
        static char pl[4096];
        static int pln = 0;
        for (int i = 0; i < n; i++) {
                char ch = b[i];
                if (ch == '\r') {
                        continue;
                } else if (ch == '\n') {
                        pl[pln] = '\0';
                        w_a(pl);
                        pln = 0;
                } else {
                        if (pln < (int)sizeof(pl) - 1) {
                                pl[pln++] = ch;
                        }
                }
        }
}

void d_t() {
        XClearWindow(d, w);
        int sl = c - r - s;
        if (sl < 0) sl = 0;
        int Y = T;
        for (int i = sl; i < c - s; i++) {
                if (l[i].t) {
                        XDrawString(d, w, g, 2, Y, l[i].t, strlen(l[i].t));
                }
                Y += T;
        }
        XFlush(d);
}

pid_t sp() {
        struct termios tp;
        if (tcgetattr(STDIN_FILENO, &tp) == -1) {
                perror("tcgetattr");
                exit(1);
        }
        pid_t pid = forkpty(&p, NULL, &tp, NULL);
        if (pid == -1) {
                perror("forkpty");
                exit(1);
        }
        if (pid == 0) {
                execlp("/usr/bin/dash", "/usr/bin/dash", NULL);
                perror("execlp");
                exit(1);
        }
        return pid;
}

/*
pid_t sp() {
	struct termios tp;
	if (tcgetattr(STDIN_FILENO, &tp) == -1) {
		perror("tcgetattr");
		exit(1);
	}
	//disable canonical mode and echo on the slave side of the pty
	tp.c_lflag &= ~(ICANON | ECHO);
	tp.c_cc[VMIN] = 1;  //minimum number of characters for noncanonical read
	tp.c_cc[VTIME] = 0; //timeout
	pid_t pid = forkpty(&p, NULL, &tp, NULL);
	if (pid == -1) {
		perror("forkpty");
		exit(1);
	}
	if (pid == 0) {
		execlp("/usr/bin/dash", "/usr/bin/dash", NULL);
		perror("execlp");
		exit(1);
	}
	return pid;
}
*/

void h_r(int Wd, int Hd) {
        x = Wd;
        y = Hd;
        z = x / F;
        r = y / T;
        char *all = NULL;
        size_t tl = 0;
        for (int i = 0; i < c; i++) {
                tl += strlen(l[i].t) + 1;
        }
        all = malloc(tl + 1);
        if (!all) {
                perror("malloc");
                exit(1);
        }
        all[0] = '\0';
        for (int i = 0; i < c; i++) {
                strcat(all, l[i].t);
                strcat(all, "\n");
        }
        f();
        p_o(all, strlen(all));
        free(all);
        s = 0;
}

int main() {
        d = XOpenDisplay(NULL);
        if (!d) {
                fprintf(stderr, "cannot open X display...\n");
                return 1;
        }
        int scr = DefaultScreen(d);
        Colormap cm = DefaultColormap(d, scr);
        XColor bc, eb;
        if (!XAllocNamedColor(d, cm, "black", &bc, &eb)) {
                fprintf(stderr, "failed to allocate hacker black...\n");
                return 1;
        }
        XColor gc, eg;
        if (!XAllocNamedColor(d, cm, "#00FF00", &gc, &eg)) {
                fprintf(stderr, "failed to allocate hacker green...\n");
                return 1;
        }
        w = XCreateSimpleWindow(d, RootWindow(d, scr),
                                     10, 10, W, H, 1,
                                     gc.pixel,
                                     bc.pixel);
        XSelectInput(d, w, ExposureMask | KeyPressMask | StructureNotifyMask);
        XMapWindow(d, w);
        XStoreName(d, w, "TERMINAX");
        XGCValues gcv;
        gcv.foreground = gc.pixel;
        gcv.background = bc.pixel;
        g = XCreateGC(d, w, GCForeground | GCBackground, &gcv);
        pid_t spid = sp();
        fd_set rfds;
        int xfd = ConnectionNumber(d);
        XEvent e;
        while (1) {
                FD_ZERO(&rfds);
                FD_SET(xfd, &rfds);
                FD_SET(p, &rfds);
                int mf = (xfd > p) ? xfd : p;
                if (select(mf + 1, &rfds, NULL, NULL, NULL) == -1) {
                        perror("select");
                        break;
                }
                if (FD_ISSET(xfd, &rfds)) {
                        while (XPending(d)) {
                                XNextEvent(d, &e);
                                if (e.type == Expose) {
                                        d_t();
                                } else if (e.type == KeyPress) {
                                        KeySym ks = XLookupKeysym(&e.xkey, 0);
                                        char ch = 0;
                                        if (ks == XK_Return) ch = '\n';
                                        else if (ks == XK_BackSpace) ch = 0x7f;
                                        else if (ks == XK_Up) {
                                                if (s < c - r)
                                                        s++;
                                                d_t();
                                        } else if (ks == XK_Down) {
                                                if (s > 0)
                                                        s--;
                                                d_t();
                                        } else if (ks < 256) {
                                                ch = (char)ks;
                                        }
                                        if (ch) {
                                                if (write(p, &ch, 1) == -1) {
                                                        perror("write to pty");
                                                        goto cl;
                                                }
                                        }
                                } else if (e.type == ConfigureNotify) {
                                        h_r(e.xconfigure.width, e.xconfigure.height);
                                        d_t();
                                }
                        }
                }
                if (FD_ISSET(p, &rfds)) {
                        char b[1024];
                        ssize_t n = read(p, b, sizeof(b));
                        if (n > 0) {
                                p_o(b, n);
                                s = 0;
                                d_t();
                        } else {
                                goto cl;
                        }
                }
        }
cl:
        f();
        close(p);
        XDestroyWindow(d, w);
        XCloseDisplay(d);
        return 0;
}
