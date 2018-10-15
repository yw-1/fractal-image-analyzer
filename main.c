#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

double maxr = 0, minr = 10000;
double maxg = 0, ming = 10000;
double maxb = 0, minb = 10000;

void ifd(SDL_Surface *src, double *fr, double *fg, double *fb, int d1, int d2)
{
	int x,y,xx,yy;
	int sx,sy,ex,ey;
	int w = src->w, h = src->h;
	int sumr, sumg, sumb;
	int meanr, meang, meanb;
	int var1r, var1g, var1b;
	int var2r, var2g, var2b;
	double logd;
	int i, j;
	size_t offset;

	logd = log(d2)-log(d1);

	Uint32 *srcpix = src->pixels;
	Uint32 z;

	for (y=0; y<h; y++) {
		for (x=0; x<w; x++) {
			sx = -d1;
			ex = d1;
			sy = -d1;
			ey = d1;

			if (x < d1) sx = x; else if (x > (w-d1)) ex = w - x;
			if (y < d1) sy = y; else if (y > (h-d1)) ey = h - y;

			sumr = 0;
			sumg = 0;
			sumb = 0;
			i = 0;
			for (yy=sy; yy<ey; yy++) {
			for (xx=sx; xx<ex; xx++) {
				z = srcpix[(src->pitch>>2)*(y+yy) + (x+xx)];

				sumr += (z>>16) & 0xff;
				sumg += (z>>8) & 0xff;
				sumb += z & 0xff;

				i++;
			}
			}
			meanr = sumr / i;
			meang = sumg / i;
			meanb = sumb / i;

			sumr = 0;
			sumg = 0;
			sumb = 0;
			i = 0;
			for (yy=sy; yy<ey; yy++) {
			for (xx=sx; xx<ex; xx++) {
				z = srcpix[(src->pitch>>2)*(y+yy) + (x+xx)];

				j = (z>>16)&0xff - meanr;
				sumr += j*j;
				j = (z>>8)&0xff - meang;
				sumg += j*j;
				j = z&0xff - meanb;
				sumb += j*j;

				i++;
			}
			}
			var1r = sumr / i;
			var1g = sumg / i;
			var1b = sumb / i;

			//***

			sx = -d2;
			ex = d2;
			sy = -d2;
			ey = d2;

			if (x < d2) sx = x; else if (x > (w-d2)) ex = w - x;
			if (y < d2) sy = y; else if (y > (h-d2)) ey = h - y;

			sumr = 0;
			sumg = 0;
			sumb = 0;
			i = 0;
			for (yy=sy; yy<ey; yy++) {
			for (xx=sx; xx<ex; xx++) {
				z = srcpix[(src->pitch>>2)*(y+yy) + (x+xx)];
				sumr += (z>>16) & 0xff;
				sumg += (z>>8) & 0xff;
				sumb += z & 0xff;

				i++;
			}
			}
			meanr = sumr / i;
			meang = sumg / i;
			meanb = sumb / i;

			sumr = 0;
			sumg = 0;
			sumb = 0;
			i = 0;
			for (yy=sy; yy<ey; yy++) {
			for (xx=sx; xx<ex; xx++) {
				z = srcpix[(src->pitch>>2)*(y+yy) + (x+xx)];

				j = (z>>16)&0xff - meanr;
				sumr += j*j;
				j = (z>>8)&0xff - meang;
				sumg += j*j;
				j = z&0xff - meanb;
				sumb += j*j;

				i++;
			}
			}
			var2r = sumr / i;
			var2g = sumg / i;
			var2b = sumb / i;

			offset = w*y + x;
			fr[offset] = (log(var2r)-log(var1r))/logd;
			fg[offset] = (log(var2g)-log(var1g))/logd;
			fb[offset] = (log(var2b)-log(var1b))/logd;

			if (fr[offset] > maxr && !isinf(fr[offset])) maxr = fr[offset];
			if (fg[offset] > maxg && !isinf(fg[offset])) maxg = fg[offset];
			if (fb[offset] > maxb && !isinf(fb[offset])) maxb = fb[offset];

			if (fr[offset] < minr && !isinf(fr[offset])) minr = fr[offset];
			if (fg[offset] < ming && !isinf(fg[offset])) ming = fg[offset];
			if (fb[offset] < minb && !isinf(fb[offset])) minb = fb[offset];
		}
	}
	printf("%f %f %f\n",maxr, maxg, maxb);
	printf("%f %f %f\n",minr, ming, minb);
}

void blitifd(double *fr, double *fg, double *fb, SDL_Surface *img, int amp)
{
	int w = img->w, h = img->h;
	int r, g, b, x, y;
	double d;
	double maxr=0, maxg=0, maxb=0;

	Uint32 *pix = img->pixels;
	Uint32 z;

	SDL_LockSurface(img);

	for (y=0; y<h; y++) {
		for (x=0; x<w; x++) {
			r = (amp*fr[w*y + x]);
			g = (amp*fg[w*y + x]);
			b = (amp*fb[w*y + x]);

			r = r*(r>0);
			g = g*(g>0);
			b = b*(b>0);

			/*
			d = fr[w*y + x]*fr[w*y + x] + fg[w*y + x]*fg[w*y + x] + fb[w*y + x]*fb[w*y + x];
			d = sqrt(d);
			r = 30*d;
			g = 30*d;
			b = 30*d;
			*/

			z = ((r&0xff)<<16) | ((g&0xff)<<8) | ((b&0xff));
			pix[(img->pitch>>2)*y + x] = z;
		}
	}
	SDL_UnlockSurface(img);
}

void plotdist(double *fr, double *fg, double *fb, SDL_Surface *img)
{
	int w = img->w, h = img->h;
	int r, g, b;
	int x, y;
	int n;

	double d;

	Uint32 *pix = img->pixels;
	Uint32 z;

	r = 255;
	g = 0;
	b = 0;

	int *plot = malloc(w * sizeof(int));

	memset(plot, 0, w*sizeof(int));

	for (y=0; y<h; y++) {
		for (x=0; x<w; x++) {
			//d = sqrt(fr[w*y + x]*fr[w*y + x] + fg[w*y + x]*fg[w*y + x] + fb[w*y + x]*fb[w*y + x]);
			d = fr[w*y + x];
			n = (d - minr)*w/(maxr - minr);
		}
	}

	int max=0, min = 10000;
	for (x=0; x<w; x++) {
		if (plot[x] > max) max = plot[x];
		if (plot[x] < min) min = plot[x];
	}

	printf("*** %d %d\n", max, min);

	SDL_LockSurface(img);

	for (x=0; x<w; x++) {
		y = h - plot[x];

		z = ((r&0xff)<<16) | ((g&0xff)<<8) | ((b&0xff));
		pix[(img->pitch>>2)*y + x] = z;
	}

	SDL_UnlockSurface(img);
}

int main(int argc, char *argv[])
{
	SDL_Window *window;
	SDL_Surface *surface;
	SDL_Event event;

	int w = 640, h = 480;
	int end = 0, redraw = 1, key;
	int mx, my;

	SDL_Init(SDL_INIT_EVERYTHING);

	int d1, d2, a;
	d1 = atoi(argv[1]);
	d2 = atoi(argv[2]);
	a = atoi(argv[3]);

	SDL_Surface *img = IMG_Load(argv[4]);
	w = img->w;
	h = img->h;

	SDL_Surface *img1 = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
	SDL_Surface *img2 = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
	SDL_BlitSurface(img, 0, img1, 0);

	window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			w, 2*h, SDL_WINDOW_SHOWN);

	surface = SDL_GetWindowSurface(window);

	double *fr = malloc(w*h*sizeof(double));
	double *fg = malloc(w*h*sizeof(double));
	double *fb = malloc(w*h*sizeof(double));
	
	ifd(img1, fr, fg, fb, d1, d2);
	blitifd(fr, fb, fg, img2, a);
	//plotdist(fr, fb, fg, img2);

	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = w;
	rect.h = h;
	SDL_BlitSurface(img1, 0, surface, &rect);
	rect.y = h;
	SDL_BlitSurface(img2, 0, surface, &rect);

	SDL_UpdateWindowSurface(window);

	while (!end) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				end = 1;
				break;
			case SDL_WINDOWEVENT:
				SDL_UpdateWindowSurface(window);
				break;
			case SDL_KEYDOWN:
				key = event.key.keysym.sym;
				if (key == SDLK_ESCAPE) end = 1;
				break;
			}

			SDL_FlushEvent(event.type);
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

