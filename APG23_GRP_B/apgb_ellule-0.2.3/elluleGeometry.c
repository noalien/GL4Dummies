#include "ellule.h"
#include "elluleRaster.h"

surface_t * elQuad(void) {  
  static triangle_t t[2] = {
    { /* Triangle 0 */
      {
	{ /* vertex 0 */
	  {-1.0f, -1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {1.0f, 0.0f, 0.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {0.0f, 0.0f} /* s, t */
	},
	{ /* vertex 1 */
	  {1.0f, -1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {0.0f, 1.0f, 0.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {1.0f, 0.0f} /* s, t */
	},
	{ /* vertex 2 */
	  {-1.0f, 1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {0.0f, 0.0f, 1.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {0.0f, 1.0f} /* s, t */
	}
      }
    },
    {
      /* Triangle 1 */
      {
	{ /* vertex 2 */
	  {-1.0f, 1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {0.0f, 0.0f, 1.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {0.0f, 1.0f} /* s, t */
	},
	{ /* vertex 1 */
	  {1.0f, -1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {0.0f, 1.0f, 0.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {1.0f, 0.0f} /* s, t */
	},
	{ /* vertex 3 */
	  {1.0f, 1.0f, 0.0f, 1.0f}, /* x, y, z, w */
	  {1.0f, 0.0f, 0.0f, 1.0f}, /* r, g, b, a */
	  {0.0f, 0.0f, 1.0f}, /* nx, ny, nz */
	  {1.0f, 1.0f} /* s, t */
	}
      }
    }
  };
  surface_t * s = malloc(sizeof *s);
  assert(s);
  s->n = 2;
  s->triangles = malloc(s->n * sizeof *(s->triangles));
  assert(s->triangles);
  memcpy(s->triangles, t, sizeof t);
  return s;
}

void elFreeSurface(surface_t * s) {
  free(s->triangles);
  free(s);
}
