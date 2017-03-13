#include <jni.h>
#include <assert.h>
#include "GL4D/gl4droid.h"

static void unitSphere(GLuint longitudes, GLuint latitudes);
static GLfloat * mkSphereVertices(GLuint longitudes, GLuint latitudes);
static GLushort * mkSphereIndex(GLuint longitudes, GLuint latitudes);

static GLuint _program;
static GLuint _vPositionHandle, _vTextureHandle, _sphereBuffers[2] = {0, 0}, _tId[2] = {0};
static GLuint _pause = 0, _parallel = 0;
static GLfloat _width = 1.0f, _height = 1.0f;
static GLfloat _ratio_x = 1.0f, _ratio_y = 1.0f, _lookAtDepth = 30.0f;
static GLfloat _lookAtRot[16] = { 1, 0, 0, 0,
                                  0, 1, 0, 0,
                                  0, 0, 1, 0,
                                  0, 0, 0, 1 };
static GLfloat _upRot[16] = { 1, 0, 0, 0,
                              0, 1, 0, 0,
                              0, 0, 1, 0,
                              0, 0, 0, 1 };

static void reshape_one_view_port(int w, int h) {
    GLfloat rx = (_width = w) / (_height = h), ry = 1.0f / rx;
    rx = MAX(rx, 1.0f);
    ry = MAX(ry, 1.0f);
    __android_log_print(ANDROID_LOG_INFO, "Rechape", "width = %f, height = %f, ratio_x = %f, ratio_y = %f\n", _width, _height, rx, ry);
    glViewport(0, 0, w, h);
    gl4duBindMatrix("projmat");
    gl4duLoadIdentityf();
    gl4duFrustumf(-rx, rx, -ry, ry, 2.0f, 1000.0f);
}

static void reshape(int w, int h) {
    const GLfloat minEyeDist = 18.0f; /* on prend des centimètres */
    const GLfloat maxEyeDist = 10000.0f; /* 100 mètres */
    const GLfloat nearSide = minEyeDist * 0.5f * 2.0f; /* pour une ouverture centrée horizontale de 60°, 2 * (sin(30°) ~ 0.5) */
    const GLfloat nearSide_2 = nearSide / 2.0f;
    if((_width = w) > (_height = h)) {
        if((w >> 1) > h) { /* n'arrive que si la largeur est plus de 2 fois la hauteur */
            _ratio_x = _width / (2.0f * _height);
            _ratio_y = 1.0f;
        } else {
            _ratio_x = 1.0f;
            _ratio_y = (2.0f * _height) / _width;
        }
    } else {
        if((h >> 1) > w) { /* n'arrive que si la hauteur est plus de 2 fois la largeur */
            _ratio_x = 1.0f;
            _ratio_y = _height / (2.0f * _width);
        } else {
            _ratio_x = (2.0f * _width) / _height;
            _ratio_y = 1.0f;
        }
    }
    gl4duBindMatrix("projmat");
    gl4duLoadIdentityf();
    gl4duFrustumf(-nearSide_2 * _ratio_x, nearSide_2 * _ratio_x, -nearSide_2 * _ratio_y, nearSide_2 * _ratio_y, minEyeDist, maxEyeDist);
}

static void updateOrientation(GLfloat ax, GLfloat ay, GLfloat az) {
    GLfloat rx[16] = {
            1,       0,        0, 0,
            0, cos(ax), -sin(ax), 0,
            0, sin(ax),  cos(ax), 0,
            0,       0,        0, 1
    };
    GLfloat ry[16] = {
            cos(ay), 0, -sin(ay), 0,
            0,       1,        0, 0,
            sin(ay), 0,  cos(ay), 0,
            0,       0,        0, 1
    };
    _upRot[0]  = cos(az); _upRot[1]  = -sin(az); _upRot[2]  = 0; _upRot[3]  = 0;
    _upRot[4]  = sin(az); _upRot[5]  =  cos(az); _upRot[6]  = 0; _upRot[7]  = 0;
    _upRot[8]  = 0; _upRot[9]  = 0; _upRot[10] = 1; _upRot[11] = 0;
    _upRot[12] = 0; _upRot[13] = 0; _upRot[14] = 0; _upRot[15] = 1;
    MMAT4XMAT4(_lookAtRot, ry, rx);
}

static void quit() {
    if(_sphereBuffers[0]) {
        glDeleteBuffers(2, _sphereBuffers);
        _sphereBuffers[0] = _sphereBuffers[1] = 0;
    }
    if(_tId[0]) {
        glDeleteTextures(sizeof _tId / sizeof *_tId, _tId);
        _tId[0] = 0;
    }
    gl4duClean(GL4DU_ALL);
    _program = 0;
    unitSphere(0, 0);
}

static int init(const char * vs, const char * fs) {
    _program = gl4droidCreateProgram(vs, fs);
    if (!_program)
        return 0;
    _vPositionHandle = glGetAttribLocation(_program, "vPosition");
    _vTextureHandle = glGetAttribLocation(_program, "vTexture");
    glClearColor(0.0f, 0.0f, 0.1f, 0.0f);
    gl4duGenMatrix(GL_FLOAT, "projmat");
    gl4duGenMatrix(GL_FLOAT, "mmat");
    gl4duGenMatrix(GL_FLOAT, "vmat");
    glGenBuffers(2, _sphereBuffers);
    glGenTextures(sizeof _tId / sizeof *_tId, _tId);
    return 1;
}

static void scene(int duplicate) {
    GLfloat mat[16], lum_pos[3] = {0.0f, 0.0f, -20.0f}, s = 1;
    static int r1 = 0, r2 = 0;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    /* Matrice du Model */
    gl4duBindMatrix("mmat");
    gl4duLoadIdentityf();
    gl4duTranslatef(0.0f, 0.0f, -30.0f);
    //gl4duRotatef(r1 * 0.1, 0, 1, 0);
    //gl4duRotatef(90, 1, 0, 0);
    gl4duRotatef(r1 * 0.1, 0, 1, 0);
    memcpy(mat, gl4duGetMatrixData(), sizeof mat);
    MMAT4INVERSE(mat);
    /* ne pas faire
       MMAT4TRANSPOSE(mat);
       car lors de l'envoi de la matrice via glUniformMatrix4fv
       le GL_TRUE pour le paramètre transpose génère une erreur sous Android
       donc on ne le fait pas et comme transpose(transpose(M)) = M, tout
       rentre dans l'ordre en passant juste M */
    lum_pos[0] = 10.0f * sin(M_PI * r2 / (180.0f * 2.0f));
    glUniformMatrix4fv(glGetUniformLocation(_program, "tinv_mmat"), 1, GL_FALSE, mat);
    glUniform3fv(glGetUniformLocation(_program, "lum_pos"), 1, lum_pos);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, _tId[1]);
    glUniform1i(glGetUniformLocation(_program, "mTex"), 0);
    glUniform1i(glGetUniformLocation(_program, "sky"), 1);
    gl4duPushMatrix();
    gl4duLoadIdentityf();
    s = 8000.0f;
    gl4duScalef(s, s, s);
    gl4duSendMatrices();
    gl4duPopMatrix();
    glFrontFace(GL_CW);
    unitSphere(29, 29);


    glBindTexture(GL_TEXTURE_2D, _tId[0]);
    glUniform1i(glGetUniformLocation(_program, "mTex"), 0);
    glUniform1i(glGetUniformLocation(_program, "sky"), 0);
    s = 8.0f;
    gl4duScalef(s, s, s);
    gl4duSendMatrices();
    glFrontFace(GL_CCW);
    unitSphere(29, 29);

    if(!_pause && !duplicate) {
        r1 += 1;
        r2 += 1;
    }
}

static void stereo(GLfloat w, GLfloat h, GLfloat dw, GLfloat dh) {
    const GLfloat eyesSapce = 6.0f; /* 6cm entre les deux yeux */
    const GLfloat eyesSapce_2 = eyesSapce / 2.0f;
    const GLfloat iUp[4] = {0, 1, 0, 1};
    GLfloat iLookAt[4] = {0, 0, -_lookAtDepth, 1}, lookAt[4], iEye[4] = {0, 0, 0, 1};
    if(!_parallel)
        MMAT4XVEC4(lookAt, _lookAtRot, iLookAt);
    /* Matrices de View */
    gl4duBindMatrix("vmat");
    gl4duLoadIdentityf();
    glViewport(0, 0, w, h);
    gl4duPushMatrix();
    if(_width > _height) {
        iEye[0] = -eyesSapce_2; iEye[1] = 0; iEye[2] = 0;
    } else {
        iEye[1] = -eyesSapce_2; iEye[0] = 0; iEye[2] = 0;
    }
    if(_parallel) {
        iLookAt[0] = iEye[0];
        iLookAt[1] = iEye[1];
        MMAT4XVEC4(lookAt, _lookAtRot, iLookAt);
    }
    gl4duLookAtf(iEye[0], iEye[1], iEye[2], lookAt[0], lookAt[1], lookAt[2], iUp[0], iUp[1], iUp[2]);
    gl4duMultMatrixf(_upRot);
    scene(0);
    gl4duBindMatrix("vmat");
    gl4duPopMatrix();
    glViewport(dw, dh, w, h);
    gl4duPushMatrix();
    if(_width > _height) {
        iEye[0] = eyesSapce_2; iEye[1] = 0; iEye[2] = 0;
    } else {
        iEye[1] = eyesSapce_2; iEye[0] = 0; iEye[2] = 0;
    }
    if(_parallel) {
        iLookAt[0] = iEye[0];
        iLookAt[1] = iEye[1];
        MMAT4XVEC4(lookAt, _lookAtRot, iLookAt);
    }
    gl4duLookAtf(iEye[0], iEye[1], iEye[2], lookAt[0], lookAt[1], lookAt[2], iUp[0], iUp[1], iUp[2]);
    gl4duMultMatrixf(_upRot);
    scene(0);
    gl4duBindMatrix("vmat");
    gl4duPopMatrix();
}

static void draw(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(_program);
    if(_width > _height)
        stereo(_width / 2.0f, _height, _width / 2.0f, 0.0f);
    else
        stereo(_width, _height / 2.0f, 0.0f, _height / 2.0f);
}

static void loadTexture(int num, int * pixels, int pw, int ph) {
    glBindTexture(GL_TEXTURE_2D, _tId[num]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pw, ph, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

static void unitSphere(GLuint longitudes, GLuint latitudes) {
    static GLuint sLongitudes = 0, sLatitudes = 0;
    if(sLongitudes != longitudes || sLatitudes != latitudes) {
        GLfloat *  sData = NULL;
        GLushort * sIndex = NULL;
        sLongitudes = longitudes;
        sLatitudes  = latitudes;
        if(!(sLongitudes * sLatitudes))
            return;
        sData  = mkSphereVertices(sLongitudes, sLatitudes);
        sIndex = mkSphereIndex(sLongitudes, sLatitudes);
        glBindBuffer(GL_ARRAY_BUFFER, _sphereBuffers[0]);
        glBufferData(GL_ARRAY_BUFFER,
                     5 * (sLongitudes + 1) * (sLatitudes + 1) * sizeof *sData, sData, GL_STATIC_DRAW);
        glVertexAttribPointer(_vPositionHandle, 3, GL_FLOAT, GL_FALSE, 5 * sizeof *sData, (const void *)0);
        glVertexAttribPointer(_vTextureHandle, 2, GL_FLOAT, GL_FALSE, 5 * sizeof *sData, (const void *)(3 * sizeof *sData));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _sphereBuffers[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     6 * sLongitudes * sLatitudes * sizeof *sIndex, sIndex, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        free(sData);
        free(sIndex);
    }
    glBindBuffer(GL_ARRAY_BUFFER, _sphereBuffers[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _sphereBuffers[1]);
    glEnableVertexAttribArray(_vPositionHandle);
    glEnableVertexAttribArray(_vTextureHandle);
    glDrawElements(GL_TRIANGLES, 6 * sLongitudes * sLatitudes, GL_UNSIGNED_SHORT, 0);
}

static GLfloat * mkSphereVertices(GLuint longitudes, GLuint latitudes) {
    int i, j, k;
    GLfloat *data;
    double theta, phi, r, y;
    double c2MPI_Long = 2.0 * M_PI / longitudes;
    double cMPI_Lat = M_PI / latitudes;
    data = malloc(5 * (longitudes + 1) * (latitudes + 1) * sizeof *data);
    assert(data);
    for(i = 0, k = 0; i <= latitudes; i++) {
        phi  = -M_PI_2 + i * cMPI_Lat;
        y = sin(phi);
        r = cos(phi);
        for(j = 0; j <= longitudes; j++) {
            theta = j * c2MPI_Long;
            data[k++] = r * cos(theta);
            data[k++] = y;
            data[k++] = r * sin(theta);
            data[k++] = 1.0 - theta / (2.0 * M_PI);
            data[k++] = 1.0 - (phi + M_PI_2) / M_PI;
        }
    }
    return data;
}

static GLushort * mkSphereIndex(GLuint longitudes, GLuint latitudes) {
    int i, ni, j, nj, k;
    GLushort * index;
    index = malloc(6 * longitudes * latitudes * sizeof *index);
    assert(index);
    for(i = 0, k = 0; i < latitudes; i++) {
        ni = i + 1;
        for(j = 0; j < longitudes; j++) {
            nj = j + 1;
            index[k++] = i * (longitudes + 1) + j;
            index[k++] = ni * (longitudes + 1) + nj;
            index[k++] = i * (longitudes + 1) + nj;

            index[k++] = i * (longitudes + 1) + j;
            index[k++] = ni * (longitudes + 1) + j;
            index[k++] = ni * (longitudes + 1) + nj;
        }
    }
    return index;
}

JNIEXPORT void JNICALL Java_com_android_SolarVR_SolarVRLib_init(JNIEnv * env, jobject obj, jstring vshader, jstring fshader) {
    char * vs = (*env)->GetStringUTFChars(env, vshader, NULL);
    char * fs = (*env)->GetStringUTFChars(env, fshader, NULL);
    init(vs, fs);
    (*env)->ReleaseStringUTFChars(env, vshader, vs);
    (*env)->ReleaseStringUTFChars(env, fshader, fs);
}

JNIEXPORT void JNICALL Java_com_android_SolarVR_SolarVRLib_loadTexture(JNIEnv * env, jobject obj, jint tex_number, jintArray pixels, jint pw, jint ph) {
    int * ppixels = NULL;
    if ((ppixels = (*env)->GetIntArrayElements(env, pixels, NULL)) != NULL) {
        loadTexture(tex_number, ppixels, pw, ph);
        (*env)->ReleaseIntArrayElements(env, pixels, ppixels, 0);
    } else
        loadTexture(tex_number, ppixels, 1, 1);
}


JNIEXPORT void JNICALL Java_com_android_SolarVR_SolarVRLib_reshape(JNIEnv * env, jobject obj,  jint width, jint height) {
    reshape(width, height);
}

JNIEXPORT void JNICALL Java_com_android_SolarVR_SolarVRLib_updateOrientation(JNIEnv * env, jobject obj,  jfloat ax, jfloat ay, jfloat az) {
    updateOrientation(ax, ay, az);
}

JNIEXPORT void JNICALL Java_com_android_SolarVR_SolarVRLib_draw(JNIEnv * env, jobject obj) {
    draw();
}

JNIEXPORT void JNICALL Java_com_android_SolarVR_SolarVRLib_quit(JNIEnv * env, jobject obj) {
    quit();
}

JNIEXPORT void JNICALL Java_com_android_SolarVR_SolarVRLib_click(JNIEnv * env, jobject obj) {
    _pause = !_pause;
}

JNIEXPORT jint JNICALL Java_com_android_SolarVR_SolarVRLib_key(JNIEnv * env, jobject obj, jint keycode, jint downOrUp) {
    if(downOrUp) /* keyup */
        return 0;
    /* keydown */
    switch(keycode) {
        case 97:
            _parallel = !_parallel;
            return 1;
        case 21:
            _lookAtDepth -= 5.0f;
            return 1;
        case 22:
            _lookAtDepth += 5.0f;
            return 1;
        default:
            break;
    }
    return 0;
}
