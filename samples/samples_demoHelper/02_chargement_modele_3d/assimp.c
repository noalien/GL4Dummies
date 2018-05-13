/*!\file assimp.c
 *
 * \brief utilisation de GL4Dummies et Lib Assimp pour chargement de
 * scènes.
 *
 * Modification de l'exemple fourni par lib Assimp utilisant GL < 3 et
 * GLUT et upgrade avec utilisation des VAO/VBO et matrices et shaders
 * GL4dummies.
 *
 * \author Vincent Boyer et Farès Belhadj {boyer, amsi}@ai.univ-paris8.fr
 * \date February 14 2017
 */

#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/* the global Assimp scene object */
static const struct aiScene* _scene = NULL;
static struct aiVector3D _scene_min, _scene_max, _scene_center;

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

static void get_bounding_box_for_node (const struct aiNode* nd, struct aiVector3D* min, struct aiVector3D* max, struct aiMatrix4x4* trafo);
static void get_bounding_box (struct aiVector3D* min, struct aiVector3D* max);
static void color4_to_float4(const struct aiColor4D *c, float f[4]);
static void set_float4(float f[4], float a, float b, float c, float d);
static void apply_material(const struct aiMaterial *mtl);
static void sceneMkVAOs (const struct aiScene *sc, const struct aiNode* nd, GLuint * ivao);
static void sceneDrawVAOs(const struct aiScene *sc, const struct aiNode* nd, GLuint * ivao);
static int  sceneNbMeshes(const struct aiScene *sc, const struct aiNode* nd, int subtotal);
static int  loadasset (const char* path);

static GLuint * _vaos = NULL, * _buffers = NULL, * _counts = NULL, * _textures = NULL, _nbMeshes = 0, _nbTextures = 0;

void assimpInit(const char * filename) {
  int i;
  GLuint ivao = 0;
  struct aiLogStream stream;
  /* get a handle to the predefined STDOUT log stream and attach
     it to the logging system. It remains active for all further
     calls to aiImportFile(Ex) and aiApplyPostProcessing. */
  stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, NULL);
  aiAttachLogStream(&stream);
  /* ... same procedure, but this stream now writes the
     log messages to assimp_log.txt */
  stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE,"assimp_log.txt");
  aiAttachLogStream(&stream);
  /* the model name can be specified on the command line. If none
     is specified, we try to locate one of the more expressive test 
     models from the repository (/models-nonbsd may be missing in 
     some distributions so we need a fallback from /models!). */
  if(loadasset(filename) != 0) {
    fprintf(stderr, "Erreur lors du chargement du fichier %s\n", filename);
    exit(3);
  } 
  /* XXX docs say all polygons are emitted CCW, but tests show that some aren't. */
  if(getenv("MODEL_IS_BROKEN"))  
    glFrontFace(GL_CW);


  _textures = malloc((_nbTextures = _scene->mNumMaterials) * sizeof *_textures);
  assert(_textures);
  
  glGenTextures(_nbTextures, _textures);

  for (i = 0; i < _scene->mNumMaterials ; i++) {
    const struct aiMaterial* pMaterial = _scene->mMaterials[i];
    if (aiGetMaterialTextureCount(pMaterial, aiTextureType_DIFFUSE) > 0) {
      struct aiString tfname;
      char * dir = pathOf(filename), buf[BUFSIZ];
      if (aiGetMaterialTexture(pMaterial, aiTextureType_DIFFUSE, 0, &tfname, NULL, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
	SDL_Surface * t;
	snprintf(buf, sizeof buf, "%s/%s", dir, tfname.data);

	if(!(t = IMG_Load(buf))) { 
	  fprintf(stderr, "Probleme de chargement de textures %s\n", buf); 
	  fprintf(stderr, "\tNouvel essai avec %s\n", tfname.data);
	  if(!(t = IMG_Load(tfname.data))) { fprintf(stderr, "Probleme de chargement de textures %s\n", tfname.data); continue; }
	}
	glBindTexture(GL_TEXTURE_2D, _textures[i]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT/* GL_CLAMP_TO_EDGE */);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT/* GL_CLAMP_TO_EDGE */);
#ifdef __APPLE__
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t->w, t->h, 0, t->format->BytesPerPixel == 3 ? GL_BGR : GL_BGRA, GL_UNSIGNED_BYTE, t->pixels);
#else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t->w, t->h, 0, t->format->BytesPerPixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, t->pixels);
#endif
	SDL_FreeSurface(t);
      }
    }
  }

  _nbMeshes= sceneNbMeshes(_scene, _scene->mRootNode, 0);
  _vaos = malloc(_nbMeshes * sizeof *_vaos);
  assert(_vaos);
  glGenVertexArrays(_nbMeshes, _vaos);
  _buffers = malloc(2 * _nbMeshes * sizeof *_buffers);
  assert(_buffers);
  glGenBuffers(2 * _nbMeshes, _buffers);
  _counts = calloc(_nbMeshes, sizeof *_counts);
  assert(_counts);
  sceneMkVAOs(_scene, _scene->mRootNode, &ivao);
}

void assimpDrawScene(void) {
  GLfloat tmp;
  GLuint ivao = 0;
  tmp = _scene_max.x - _scene_min.x;
  tmp = aisgl_max(_scene_max.y - _scene_min.y, tmp);
  tmp = aisgl_max(_scene_max.z - _scene_min.z, tmp);
  tmp = 1.0f / tmp;
  gl4duScalef(tmp, tmp, tmp);
  gl4duTranslatef( -_scene_center.x, -_scene_center.y, -_scene_center.z);
  sceneDrawVAOs(_scene, _scene->mRootNode, &ivao);
}

void assimpQuit(void) {
  /* cleanup - calling 'aiReleaseImport' is important, as the library 
     keeps internal resources until the scene is freed again. Not 
     doing so can cause severe resource leaking. */
  aiReleaseImport(_scene);
  /* We added a log stream to the library, it's our job to disable it
     again. This will definitely release the last resources allocated
     by Assimp.*/
  aiDetachAllLogStreams();
  if(_counts) {
    free(_counts);
    _counts = NULL;
  }
  if(_textures) {
    glDeleteTextures(_nbTextures, _textures);
    free(_textures);
    _textures = NULL;
  }
  if(_vaos) {
    glDeleteVertexArrays(_nbMeshes, _vaos);
    free(_vaos);
    _vaos = NULL;
  }
  if(_buffers) {
    glDeleteBuffers(2 * _nbMeshes, _buffers);
    free(_buffers);
    _buffers = NULL;
  }
}

static void get_bounding_box_for_node(const struct aiNode* nd, struct aiVector3D* min, struct aiVector3D* max, struct aiMatrix4x4* trafo) {
  struct aiMatrix4x4 prev;
  unsigned int n = 0, t;
  prev = *trafo;
  aiMultiplyMatrix4(trafo,&nd->mTransformation);
  for (; n < nd->mNumMeshes; ++n) {
    const struct aiMesh* mesh = _scene->mMeshes[nd->mMeshes[n]];
    for (t = 0; t < mesh->mNumVertices; ++t) {
      struct aiVector3D tmp = mesh->mVertices[t];
      aiTransformVecByMatrix4(&tmp,trafo);
      min->x = aisgl_min(min->x,tmp.x);
      min->y = aisgl_min(min->y,tmp.y);
      min->z = aisgl_min(min->z,tmp.z);
      max->x = aisgl_max(max->x,tmp.x);
      max->y = aisgl_max(max->y,tmp.y);
      max->z = aisgl_max(max->z,tmp.z);
    }
  }
  for (n = 0; n < nd->mNumChildren; ++n) {
    get_bounding_box_for_node(nd->mChildren[n],min,max,trafo);
  }
  *trafo = prev;
}

static void get_bounding_box (struct aiVector3D* min, struct aiVector3D* max) {
  struct aiMatrix4x4 trafo;
  aiIdentityMatrix4(&trafo);
  min->x = min->y = min->z =  1e10f;
  max->x = max->y = max->z = -1e10f;
  get_bounding_box_for_node(_scene->mRootNode,min,max,&trafo);
}

static void color4_to_float4(const struct aiColor4D *c, float f[4]) {
  f[0] = c->r; f[1] = c->g; f[2] = c->b; f[3] = c->a;
}

static void set_float4(float f[4], float a, float b, float c, float d) {
  f[0] = a; f[1] = b; f[2] = c; f[3] = d;
}

static void apply_material(const struct aiMaterial *mtl) {
  float c[4];
  unsigned int max;
  float shininess, strength;
  struct aiColor4D diffuse, specular, ambient, emission;
  GLint id;
  glGetIntegerv(GL_CURRENT_PROGRAM, &id);
  
  set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
  if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse)){
    color4_to_float4(&diffuse, c);
  }
  glUniform4fv(glGetUniformLocation(id, "diffuse_color"), 1, c);

  set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
  if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular)){
    color4_to_float4(&specular, c);
  }
  glUniform4fv(glGetUniformLocation(id, "specular_color"), 1, c);

  set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
  if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient)){
    color4_to_float4(&ambient, c);
  }
  glUniform4fv(glGetUniformLocation(id, "ambient_color"), 1, c);

  set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
  if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission)){
    color4_to_float4(&emission, c);
  }
  glUniform4fv(glGetUniformLocation(id, "emission_color"), 1, c);

  max = 1;
  if(aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max) == AI_SUCCESS) {
    max = 1;
    if(aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max) == AI_SUCCESS)
      glUniform1f(glGetUniformLocation(id, "shininess"), shininess * strength);
    else
	glUniform1f(glGetUniformLocation(id, "shininess"), shininess);
  } else {
    shininess = 0.0;
    glUniform1f(glGetUniformLocation(id, "shininess"), shininess);
  }
}

static void sceneMkVAOs(const struct aiScene *sc, const struct aiNode* nd, GLuint * ivao) {
  int i, j, comp;
  unsigned int n = 0;
  static int temp = 0;
  
  temp++;

  for (; n < nd->mNumMeshes; ++n) {
    GLfloat * vertices = NULL;
    GLuint  * indices  = NULL;
    const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];
    comp  = mesh->mVertices ? 3 : 0;
    comp += mesh->mNormals ? 3 : 0;
    comp += mesh->mTextureCoords[0] ? 2 : 0;
    if(!comp) continue;

    glBindVertexArray(_vaos[*ivao]);
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[2 * (*ivao)]);

    vertices = malloc(comp * mesh->mNumVertices * sizeof *vertices);
    assert(vertices);
    i = 0;
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    if(mesh->mVertices) {
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)(i * sizeof *vertices));
      for(j = 0; j < mesh->mNumVertices; ++j) {
	vertices[i++] = mesh->mVertices[j].x;
	vertices[i++] = mesh->mVertices[j].y;
	vertices[i++] = mesh->mVertices[j].z;
      }      
    }
    if(mesh->mNormals) {
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const void *)(i * sizeof *vertices));
      for(j = 0; j < mesh->mNumVertices; ++j) {
	vertices[i++] = mesh->mNormals[j].x;
	vertices[i++] = mesh->mNormals[j].y;
	vertices[i++] = mesh->mNormals[j].z;
      }      
    }
    if(mesh->mTextureCoords[0]) {
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)(i * sizeof *vertices));
      for(j = 0; j < mesh->mNumVertices; ++j) {
	vertices[i++] = mesh->mTextureCoords[0][j].x;
	vertices[i++] = mesh->mTextureCoords[0][j].y;
      }      
    }
    glBufferData(GL_ARRAY_BUFFER, (i * sizeof *vertices), vertices, GL_STATIC_DRAW);      
    free(vertices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers[2 * (*ivao) + 1]);
    if(mesh->mFaces) {
      indices = malloc(3 * mesh->mNumFaces * sizeof *indices);
      assert(indices);
      for(i = 0, j = 0; j < mesh->mNumFaces; ++j) {
	assert(mesh->mFaces[j].mNumIndices < 4);
	if(mesh->mFaces[j].mNumIndices != 3) continue;
	indices[i++] = mesh->mFaces[j].mIndices[0];
	indices[i++] = mesh->mFaces[j].mIndices[1];
	indices[i++] = mesh->mFaces[j].mIndices[2];
      }
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, i * sizeof *indices, indices, GL_STATIC_DRAW);
      _counts[*ivao] = i;
      free(indices);
    }
    glBindVertexArray(0);
    (*ivao)++;
  }
  for (n = 0; n < nd->mNumChildren; ++n) {
    sceneMkVAOs(sc, nd->mChildren[n], ivao);
  }
}


static void sceneDrawVAOs(const struct aiScene *sc, const struct aiNode* nd, GLuint * ivao) {
  unsigned int n = 0;
  struct aiMatrix4x4 m = nd->mTransformation;
  GLint id;

  glGetIntegerv(GL_CURRENT_PROGRAM, &id);
  /* By VB Inutile de transposer la matrice, gl4dummies fonctionne avec des transpose de GL. */
  /* aiTransposeMatrix4(&m); */
  gl4duPushMatrix();
  gl4duMultMatrixf((GLfloat*)&m);
  gl4duSendMatrices();

  for (; n < nd->mNumMeshes; ++n) {
    const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];
    if(_counts[*ivao]) {
      glBindVertexArray(_vaos[*ivao]);
      apply_material(sc->mMaterials[mesh->mMaterialIndex]);
      if (aiGetMaterialTextureCount(sc->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE) > 0) {
	glBindTexture(GL_TEXTURE_2D, _textures[mesh->mMaterialIndex]);
	glUniform1i(glGetUniformLocation(id, "hasTexture"), 1);
	glUniform1i(glGetUniformLocation(id, "myTexture"), 0);
      } else {
	glUniform1i(glGetUniformLocation(id, "hasTexture"), 0);
      }
      glDrawElements(GL_TRIANGLES, _counts[*ivao], GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
    (*ivao)++;
  }  
  for (n = 0; n < nd->mNumChildren; ++n) {
    sceneDrawVAOs(sc, nd->mChildren[n], ivao);
  }
  gl4duPopMatrix(); 
}

static int sceneNbMeshes(const struct aiScene *sc, const struct aiNode* nd, int subtotal) {
  int n = 0;
  subtotal += nd->mNumMeshes;
  for(n = 0; n < nd->mNumChildren; ++n)
    subtotal += sceneNbMeshes(sc, nd->mChildren[n], 0);
  return subtotal;
}

static int loadasset (const char* path) {
  /* we are taking one of the postprocessing presets to avoid
     spelling out 20+ single postprocessing flags here. */
  /* struct aiString str; */
  /* aiGetExtensionList(&str); */
  /* fprintf(stderr, "EXT %s\n", str.data); */
  _scene = aiImportFile(path, 
		       aiProcessPreset_TargetRealtime_MaxQuality |
		       aiProcess_CalcTangentSpace       |
		       aiProcess_Triangulate            |
		       aiProcess_JoinIdenticalVertices  |
		       aiProcess_SortByPType);
  if (_scene) {
    get_bounding_box(&_scene_min,&_scene_max);
    _scene_center.x = (_scene_min.x + _scene_max.x) / 2.0f;
    _scene_center.y = (_scene_min.y + _scene_max.y) / 2.0f;
    _scene_center.z = (_scene_min.z + _scene_max.z) / 2.0f;
    return 0;
  }
  return 1;
}
