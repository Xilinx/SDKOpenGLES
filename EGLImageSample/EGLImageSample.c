/*******************************************************************************************************************************************

  Initial Version - 08/22/2017 - Email - alok.gupta@xilinx.com
  Xilinx Inc.

  Test Application using EGL Image

 *******************************************************************************************************************************************/


//Include Header Files
/*********************************************************************************************************************************************************/
#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <EGL/egl.h>
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/eglext.h>
#include <GLES2/gl2.h>  /* use OpenGL ES 2.x */
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2ext.h>  /* use OpenGL ES 2.x extension */
#include <drm/drm_fourcc.h>
#include <linux/videodev2.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//Static/Global Variables
/*********************************************************************************************************************************************************/


EGLClientBuffer clientBuffer;
GLuint source,source2;
GLuint targetTexture,targetTexture2;
GLubyte originalData[4] = {0, 255, 0, 0};
GLubyte updateData[4]   = {255, 0, 0, 0};
GLuint fragShader, vertShader, program;
#define M_PI 3.14
float time = 0.0;
int memfd;
void *mapped_base;
unsigned char *p = NULL;//(unsigned char *)mapped_base;
EGLImageKHR egl_img,egl_img2;

typedef int bool;
#define TRUE  1
#define FALSE 0

static const GLfloat verts[6][2] = {
	{ -1, -1 },
	{ -1,  1 },
	{  1, -1 },
	{ -1,  1 },
	{  1,  1 },
	{  1, -1 }
};
static const GLfloat tex_coord[6][2] = {
	{  0, 1 },
	{  0, 0 },
	{  1, 1 },
	{  0, 0 },
	{  1, 0 },
	{  1, 1 }
};



#define FLOAT_TO_FIXED(X)   ((X) * 65535.0)

static GLfloat view_rotx = 0.0, view_roty = 0.0;

static GLint u_matrix = -1;
static GLint attr_pos = 0, attr_color = 1, attr_a_texcoord = 2;
static GLuint tex;

#define TEX_WIDTH	1920
#define TEX_HEIGHT	1080

static tex_width = TEX_WIDTH;
static tex_height = TEX_HEIGHT;


//Function Statrts
/*********************************************************************************************************************************************************/

static void CheckError(int line)
{
	GLenum err = glGetError();
	if (err) {
		printf("*** GL Error 0x%x at line %d ***\n", (int) err, line);
	}
	EGLint eerr = eglGetError();
	if (eerr != EGL_SUCCESS) {
		printf("*** EGL Error 0x%x at line %d ***\n", (int) eerr, line);
	}
}

//Function Ends
/*********************************************************************************************************************************************************/


//Function Statrts
/*********************************************************************************************************************************************************/
static void make_z_rot_matrix(GLfloat angle, GLfloat *m)
{
	float c = cos(angle * M_PI / 180.0);
	float s = sin(angle * M_PI / 180.0);
	int i;
	for (i = 0; i < 16; i++)
		m[i] = 0.0;
	m[0] = m[5] = m[10] = m[15] = 1.0;

	m[0] = c;
	m[1] = s;
	m[4] = -s;
	m[5] = c;
}
//Function Ends
/*********************************************************************************************************************************************************/



//Function Statrts
/*********************************************************************************************************************************************************/
static void make_scale_matrix(GLfloat xs, GLfloat ys, GLfloat zs, GLfloat *m)
{
	int i;
	for (i = 0; i < 16; i++)
		m[i] = 0.0;
	m[0] = xs;
	m[5] = ys;
	m[10] = zs;
	m[15] = 1.0;
}
//Function Ends
/*********************************************************************************************************************************************************/




//Function Statrts
/*********************************************************************************************************************************************************/
static void mul_matrix(GLfloat *prod, const GLfloat *a, const GLfloat *b)
{
#define A(row,col)  a[(col<<2)+row]
#define B(row,col)  b[(col<<2)+row]
#define P(row,col)  p[(col<<2)+row]
	GLfloat p[16];
	GLint i;
	for (i = 0; i < 4; i++) {
		const GLfloat ai0=A(i,0),  ai1=A(i,1),  ai2=A(i,2),  ai3=A(i,3);
		P(i,0) = ai0 * B(0,0) + ai1 * B(1,0) + ai2 * B(2,0) + ai3 * B(3,0);
		P(i,1) = ai0 * B(0,1) + ai1 * B(1,1) + ai2 * B(2,1) + ai3 * B(3,1);
		P(i,2) = ai0 * B(0,2) + ai1 * B(1,2) + ai2 * B(2,2) + ai3 * B(3,2);
		P(i,3) = ai0 * B(0,3) + ai1 * B(1,3) + ai2 * B(2,3) + ai3 * B(3,3);
	}
	memcpy(prod, p, sizeof(p));
#undef A
#undef B
#undef PROD
}
//Function Ends
/*********************************************************************************************************************************************************/


void prepareEGLImageSource(EGLDisplay egl_dpy, EGLContext egl_ctx,size_t width,
		size_t height,
		GLenum format,
		GLenum type,
		void *data,
		GLuint *outSourceTexture,
		EGLImageKHR *outSourceImage)
{
	// Create a source 2D texture
	GLuint wsource;
	glGenTextures(1, &wsource);
	glBindTexture(GL_TEXTURE_2D, wsource);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width,height, 0, format, type, data);
	// Disable mipmapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Create an image from the source texture
	EGLint imageAttributes[] = { 
		EGL_GL_TEXTURE_LEVEL_KHR, 0,
		EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, 
		EGL_NONE 
	};

	EGLImageKHR image = eglCreateImageKHR(egl_dpy, egl_ctx, EGL_GL_TEXTURE_2D_KHR,(EGLClientBuffer)wsource, imageAttributes);

	*outSourceTexture = wsource;
	*outSourceImage   = image;
}


void prepareEGLImageTarget(EGLImageKHR image, GLuint *outTargetTexture)
{
	// Create a target texture from the image
	GLuint target;
	glGenTextures(1, &target);
	glBindTexture(GL_TEXTURE_EXTERNAL_OES, target);
	glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, image);
	// Disable mipmapping
	glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	*outTargetTexture = target;
}




//Function Statrts
/*********************************************************************************************************************************************************/
static void draw_texture(bool update)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(!update)	//originalData
	{	

		GLfloat mat[16], rot[16], scale[16];
		make_z_rot_matrix(view_rotx, rot);
		make_scale_matrix(1, 1, 1, scale);
		mul_matrix(mat, rot, scale);
		glUniformMatrix4fv(u_matrix, 1, GL_FALSE, mat);
		glUseProgram(program);
		glBindTexture(GL_TEXTURE_EXTERNAL_OES, targetTexture);
		glVertexAttribPointer(attr_pos, 2, GL_FLOAT, GL_FALSE, 0, verts);
		glVertexAttribPointer(attr_a_texcoord, 2, GL_FLOAT, GL_FALSE, 0, tex_coord);
		glEnableVertexAttribArray(attr_pos);
		glEnableVertexAttribArray(attr_a_texcoord);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		GLubyte pixel_color[3];
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel_color[0]);
	}
	else	//Updated Data
	{

		glBindTexture(GL_TEXTURE_EXTERNAL_OES, targetTexture2);
		glVertexAttribPointer(attr_pos, 2, GL_FLOAT, GL_FALSE, 0, verts);
		glVertexAttribPointer(attr_a_texcoord, 2, GL_FLOAT, GL_FALSE, 0, tex_coord);
		glEnableVertexAttribArray(attr_pos);
		glEnableVertexAttribArray(attr_a_texcoord);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		GLubyte pixel_color[3];
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel_color[0]);

	}
	printf("\n");
}
//Function Ends
/*********************************************************************************************************************************************************/




//Function Statrts
/*********************************************************************************************************************************************************/
static void create_shaders_texture(void)
{
	static const char *fragShaderText =
		"#extension GL_OES_EGL_image_external : require\n"
		"precision mediump float;\n"
		"uniform samplerExternalOES texture;\n"
		"varying vec2 v_texcoord;\n"
		"void main() {							\n"
		"	 gl_FragColor = texture2D(texture, v_texcoord);\n"
		"}\n";
	static const char *vertShaderText =
		"uniform mat4 modelviewProjection;\n"
		"attribute vec4 pos;\n"
		"attribute vec2 a_texcoord;\n"
		"varying vec2 v_texcoord;\n"
		"void main() {\n"
		"   gl_Position = modelviewProjection * pos;\n"
		"   v_texcoord = a_texcoord;\n"
		"}\n";

	GLint stat;

#define LOG_LEN	1000
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, (const char **) &fragShaderText, NULL);
	glCompileShader(fragShader);
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &stat);
	if (!stat) {
		char log[LOG_LEN];
		GLsizei len;
		glGetShaderInfoLog(fragShader, LOG_LEN, &len, log);
		printf("%s\n", log);
		printf("Error: fragment shader did not compile!\n");
		exit(1);
	}

	vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, (const char **) &vertShaderText, NULL);
	glCompileShader(vertShader);
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &stat);
	if (!stat) {
		char log[LOG_LEN];
		GLsizei len;
		glGetShaderInfoLog(vertShader, LOG_LEN, &len, log);
		printf("%s\n", log);
		printf("Error: vertex shader did not compile!\n");
		exit(1);
	}

	program = glCreateProgram();
	glAttachShader(program, fragShader);
	glAttachShader(program, vertShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &stat);
	if (!stat) {
		char log[LOG_LEN];
		GLsizei len;
		glGetProgramInfoLog(program, LOG_LEN, &len, log);
		printf("Error: linking:\n%s\n", log);
		exit(1);
	}

	glUseProgram(program);

	if (1) {
		/* test setting attrib locations */
		glBindAttribLocation(program, attr_pos, "pos");
		glBindAttribLocation(program, attr_a_texcoord, "a_texcoord");
		glLinkProgram(program);  /* needed to put attribs into effect */
	}
	else {
		/* test automatic attrib locations */
		attr_pos = glGetAttribLocation(program, "pos");
		attr_a_texcoord = glGetAttribLocation(program, "a_texcoord");
	}

	u_matrix = glGetUniformLocation(program, "modelviewProjection");
}
//Function Ends
/*********************************************************************************************************************************************************/





//Function Statrts
/*********************************************************************************************************************************************************/
void prepareStaticMem(const char *dev_name)
{
	int fd, ret;
	memfd = open(dev_name, O_RDWR | O_SYNC);
	if (fd < 0)
		printf("%s %d %d\n", __FUNCTION__, __LINE__, fd);
	mapped_base = mmap(0x3e000000, 0x1000000, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, 0);
	if (mapped_base == MAP_FAILED){
		printf("%s %d %d\n", __FUNCTION__, __LINE__, ret);
	}
}
//Function Ends
/*********************************************************************************************************************************************************/





//Function Statrts
/*********************************************************************************************************************************************************/
static void make_fbdev_window(Display *x_dpy, EGLDisplay egl_dpy,
		const char *name,
		int x, int y, int width, int height,
		EGLContext *ctxRet,
		EGLSurface *surfRet)
{
	const EGLNativeWindowType native_win = (EGLNativeWindowType) NULL;
	static EGLContext egl_ctx;
	static EGLSurface egl_surf;
	EGLint major, minor, num_configs;
	EGLConfig conf;
	EGLint attrib[32], i = 0;
	struct fb_var_screeninfo vinfo;
	int fd, egl_verbose;

	fd = open("/dev/fb0", O_RDWR);
	if (fd < 0)
		printf("failed to open");

	memset(&vinfo, 0, sizeof(vinfo));
	if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo))
		printf("failed to get fb info");

	egl_verbose = 1;

	egl_dpy = eglGetDisplay((EGLNativeDisplayType) EGL_DEFAULT_DISPLAY);
	if (egl_dpy == EGL_NO_DISPLAY)
		printf("failed to get a display");
	if (!eglInitialize(egl_dpy, &major, &minor))
		printf("failed to initialize EGL");

	if (egl_verbose) {
		printf("\nEGL %d.%d\n", major, minor);
		printf("\nEGL_VENDOR: %s\n", eglQueryString(egl_dpy, EGL_VENDOR));
		printf("\nEGL_VERSION: %s\n", eglQueryString(egl_dpy, EGL_VERSION));
		printf("\nEGL_EXTENSIONS: %s\n", eglQueryString(egl_dpy, EGL_EXTENSIONS));
		printf("\nEGL_CLIENT_APIS: %s\n",
				eglQueryString(egl_dpy, EGL_CLIENT_APIS));
	}

	attrib[i++] = EGL_RENDERABLE_TYPE;
	attrib[i++] = EGL_OPENGL_ES2_BIT;
	attrib[i++] = EGL_BUFFER_SIZE;
	attrib[i++] = vinfo.bits_per_pixel;
	attrib[i++] = EGL_NONE;
	if (!eglChooseConfig(egl_dpy, attrib, &conf, 1, &num_configs) ||
			!num_configs)
		printf("failed to choose a config");

	if (egl_verbose) {
		EGLint tmp;
		eglGetConfigAttrib(egl_dpy, conf, EGL_BUFFER_SIZE, &tmp);
		eglGetConfigAttrib(egl_dpy, conf, EGL_RED_SIZE, &tmp);
		eglGetConfigAttrib(egl_dpy, conf, EGL_GREEN_SIZE, &tmp);
		eglGetConfigAttrib(egl_dpy, conf, EGL_BLUE_SIZE, &tmp);
		eglGetConfigAttrib(egl_dpy, conf, EGL_CONFIG_ID, &tmp);
	}

	i = 0;
	attrib[i++] = EGL_CONTEXT_CLIENT_VERSION;
	attrib[i++] = 2;
	attrib[i] = EGL_NONE;

	egl_ctx = eglCreateContext(egl_dpy, conf, EGL_NO_CONTEXT, attrib);
	if (egl_ctx == EGL_NO_CONTEXT)
		printf("failed to create a context");

	egl_surf = eglCreateWindowSurface(egl_dpy, conf, native_win, NULL);
	if (egl_surf == EGL_NO_SURFACE)
		printf("failed to create a surface");

	if (!eglMakeCurrent(egl_dpy, egl_surf, egl_surf, egl_ctx))
		printf("failed to make context/surface current");
	*ctxRet = egl_ctx;
	*surfRet = egl_surf;

}
//Function Ends
/*********************************************************************************************************************************************************/



//Function Statrts
/*********************************************************************************************************************************************************/
static void event_loop(EGLDisplay egl_dpy, EGLSurface egl_surf, bool update)
{
	draw_texture(update);//First time write
	eglSwapBuffers(egl_dpy, egl_surf);
}
//Function Ends
/*********************************************************************************************************************************************************/


//Function Statrts
/*********************************************************************************************************************************************************/
void prepareEGLImg(EGLDisplay egl_dpy, EGLContext egl_ctx){

	memset(mapped_base,0, 5000);	//Copy at Static Mem location
	memcpy(mapped_base,originalData,sizeof(originalData));

	p = (unsigned char *)mapped_base;
	printf("\n");

	EGLint egl_img_attr[] = { 
		EGL_GL_TEXTURE_LEVEL_KHR, 0,
		EGL_IMAGE_PRESERVED_KHR, EGL_TRUE,
	};


	prepareEGLImageSource(egl_dpy,egl_ctx,1, 1, GL_RGBA, GL_UNSIGNED_BYTE, mapped_base, &source, &egl_img);
	memcpy(mapped_base,updateData,sizeof(updateData));
	prepareEGLImageSource(egl_dpy,egl_ctx,1, 1, GL_RGBA, GL_UNSIGNED_BYTE, mapped_base, &source2, &egl_img2);
	CheckError(__LINE__);
	prepareEGLImageTarget(egl_img, &targetTexture);
	prepareEGLImageTarget(egl_img2, &targetTexture2);
	// Delete the source texture
	CheckError(__LINE__);
	create_shaders_texture();

}
//Function Ends
/*********************************************************************************************************************************************************/



//Main Function Statrts
/*********************************************************************************************************************************************************/
int main(int argc, char *argv[])
{
	const int winWidth = 1920, winHeight = 1080 ;
	Display *x_dpy;
	Window win;
	EGLSurface egl_surf;
	EGLContext egl_ctx;
	EGLDisplay egl_dpy;
	char *dpyName = NULL;
	char *devname = NULL;
	GLboolean printInfo = GL_FALSE;
	EGLint egl_major, egl_minor;
	int i;
	const char *s;


	prepareStaticMem("/dev/mem");
	x_dpy = EGL_DEFAULT_DISPLAY;
	egl_dpy = eglGetDisplay(x_dpy);

	if (!egl_dpy) {
		printf("Error: eglGetDisplay() failed\n");
		return -1;
	}


	if (!eglInitialize(egl_dpy, &egl_major, &egl_minor)) {
		printf("\nError: eglInitialize() failed\n");
		return -1;
	}

	s = eglQueryString(egl_dpy, EGL_VERSION);
	printf("\nEGL_VERSION = %s\n", s);

	s = eglQueryString(egl_dpy, EGL_VENDOR);
	printf("\nEGL_VENDOR = %s\n", s);


	s = eglQueryString(egl_dpy, EGL_EXTENSIONS);
	printf("\nEGL_EXTENSIONS = %s\n", s);


	s = eglQueryString(egl_dpy, EGL_CLIENT_APIS);
	printf("\nEGL_CLIENT_APIS = %s\n", s);


	make_fbdev_window(x_dpy, egl_dpy,"OpenGL ES 2.0 App", 0, 0, winWidth, winHeight,&egl_ctx, &egl_surf);


	if (!eglMakeCurrent(egl_dpy, egl_surf, egl_surf, egl_ctx)) {
		printf("\nError: eglMakeCurrent() failed\n");
		return -1;
	}


	if (printInfo) {
		printf("\nGL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
		printf("\nGL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
		printf("\nGL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
		printf("\nGL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
	}

	prepareEGLImg(egl_dpy,egl_ctx);

	bool finish = GL_FALSE;
	static int j=0;
	while(!finish)
	{
		event_loop(egl_dpy, egl_surf,FALSE);	//update 0 -- Write Some color
		sleep(1);
		event_loop(egl_dpy, egl_surf,TRUE);	//update 1 
		sleep(1);
		j++;
		if(j==5)
			finish = GL_TRUE;
	}

	eglDestroyContext(egl_dpy, egl_ctx);
	eglDestroySurface(egl_dpy, egl_surf);
	eglTerminate(egl_dpy);

	return 0;
}
//Main Function Ends
/*********************************************************************************************************************************************************/



