/*!\file window.c
 *
 * \brief Labyrinth generator.
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 20 2018
 */
#include <stdlib.h>
#include <assert.h>

static void propoagate(int * lab, int v, int x, int y, int w, int * n) {
  int i;
  const int dir[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
  if(v < 0)
    return;
  if(!v && (x&1 && y&1))
    --(*n);
  lab[y * w + x] = v;
  for(i = 0; i < 4; ++i)
    if(lab[(y + dir[i][1]) * w + x + dir[i][0]] > v)
      propoagate(lab, v, x + dir[i][0], y + dir[i][1], w, n);
}

unsigned int * labyrinth(int w, int h) {
  int i, j, k = 0, sw = (w - 1) / 2, sh = (h - 1) / 2;
  int * lab, toGo = sw * sh - 1;
  int mx, my, d;
  assert((w&1) && (h&1));
  lab = malloc(w * h * sizeof *lab);
  assert(lab);
  for(i = 0; i < h; ++i)
    for(j = 0; j< w; ++j)
      if((i&1) && (j&1))
	lab[i * w + j] = k++;
      else
	lab[i * w + j] = -1;
  while(toGo > 0) {
    do {
      if(rand() & 1) { /* LR */
	mx = 1 + 2 * (rand() % sw);
	my = 2 * (1 + (rand() % (sh - 1)));
      } else { /* TB */
	my = 1 + 2 * (rand() % sh);
	mx = 2 * (1 + (rand() % (sw - 1)));
      }
    } while((lab[my * w + mx] != -1));
    if(mx&1) {
      d = lab[(my - 1) * w + mx] - lab[(my + 1) * w + mx]; 
      if(d > 0) {
	lab[my * w + mx] = lab[(my + 1) * w + mx];
	propoagate(lab, lab[my * w + mx], mx, my - 1, w, &toGo);
      } else if(d < 0) {
	lab[my * w + mx] = lab[(my - 1) * w + mx];
	propoagate(lab, lab[my * w + mx], mx, my + 1, w, &toGo);
      }
    } else {
      d = lab[my * w + mx - 1] - lab[my * w + mx + 1]; 
      if(d > 0) {
	lab[my * w + mx] = lab[my * w + mx + 1];
	propoagate(lab, lab[my * w + mx], mx - 1, my, w, &toGo);
      } else if(d < 0) {
	lab[my * w + mx] = lab[my * w + mx - 1];
	propoagate(lab, lab[my * w + mx], mx + 1, my, w, &toGo);
      }
    }
  }
  return (unsigned int *)lab;
}
