/*******************************************************************************************************************************************
 /*******************************************************************************
 *
 * Copyright (C) 2023 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * XILINX CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
*******************************************************************************
  Video Texture Application using EGL Image
  Assumption : /dev/video0 is Video Node where streaming texture is coming 
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

//MACROS
#define BUF_SIZE  4
#define LOG_LEN 1000
#define FLOAT_TO_FIXED(X)   ((X) * 65535.0)
#define TEX_WIDTH       (1920)
#define TEX_HEIGHT      (1080)

//Static/Global Variables
/*********************************************************************************************************************************************************/
static GLfloat view_rotx = 0.5, view_roty = 0.5;
static GLint u_matrix = -1;
static GLint attr_pos = 0, attr_color = 1, attr_a_texcoord = 2;
static GLuint tex;
static tex_width = TEX_WIDTH;
static tex_height = TEX_HEIGHT;
struct EGLImgpack {
        unsigned int index;
        int dbuf_fd;
        EGLImageKHR egl_img;
        void *buf;
        unsigned int length;
};
struct stream_buf {
        int fd;
        struct EGLImgpack EGLcushion[BUF_SIZE];
};



//Function Statrts
/*********************************************************************************************************************************************************/
static void CheckError(int line)
{
   GLenum err = glGetError();
   if (err) {
      printf("## OpenGL Error 0x%x at line %d ##\n", (int) err, line);
   }
   EGLint eerr = eglGetError();
   if (eerr != EGL_SUCCESS) {
      printf("## EGL Error 0x%x at line %d ##\n", (int) eerr, line);
   }
}
//Function Ends
/*********************************************************************************************************************************************************/



//Function Statrts
/*********************************************************************************************************************************************************/
static void
make_z_rot_matrix(GLfloat angle, GLfloat *m)
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
static void
make_scale_matrix(GLfloat xs, GLfloat ys, GLfloat zs, GLfloat *m)
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
static void
mul_matrix(GLfloat *prod, const GLfloat *a, const GLfloat *b)
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



//Function Statrts
/*********************************************************************************************************************************************************/
static void
draw_texture(struct EGLImgpack *EGLcushion)
{
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
   GLfloat mat[16], rot[16], scale[16];
   /* Set modelview/projection matrix */
   make_z_rot_matrix(view_rotx, rot);
   make_scale_matrix(.5, .5, .5, scale);
   mul_matrix(mat, rot, scale);
   glUniformMatrix4fv(u_matrix, 1, GL_FALSE, mat);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_EXTERNAL_OES, tex);
   glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, EGLcushion->egl_img);
   glVertexAttribPointer(attr_pos, 2, GL_FLOAT, GL_FALSE, 0, verts);
   glVertexAttribPointer(attr_a_texcoord, 2, GL_FLOAT, GL_FALSE, 0, tex_coord);
   glEnableVertexAttribArray(attr_pos);
   glEnableVertexAttribArray(attr_a_texcoord);
   glDrawArrays(GL_TRIANGLES, 0, 6);
   glDisableVertexAttribArray(attr_pos);
   glDisableVertexAttribArray(attr_a_texcoord);

}
//Function Ends
/*********************************************************************************************************************************************************/


//Function Statrts
/*********************************************************************************************************************************************************/
static void
reshape(int width, int height)
{
   glViewport(0, 0, (GLint) width, (GLint) height);
}
//Function Ends
/*********************************************************************************************************************************************************/



//Function Statrts
/*********************************************************************************************************************************************************/
static void
create_shaders_texture(void)
{
   static const char *fragShaderText =
      "#extension GL_OES_EGL_image_external : require\n"
      "precision mediump float;\n"
      "uniform samplerExternalOES texture;\n"
      "varying vec2 v_texcoord;\n"
      "void main() {\n"
      "   gl_FragColor = texture2D(texture, v_texcoord);\n"
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

   GLuint fragShader, vertShader, program;
   GLint stat;

   fragShader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragShader, 1, (const char **) &fragShaderText, NULL);
   glCompileShader(fragShader);
   glGetShaderiv(fragShader, GL_COMPILE_STATUS, &stat);
   if (!stat) {
      char log[LOG_LEN];
      GLsizei len;
      glGetShaderInfoLog(fragShader, LOG_LEN, &len, log);
      printf("%s\n", log);
      printf("Error: fragment shader compilation failed!\n");
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
      printf("Error: vertex shader compilation failed!\n");
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
   glBindAttribLocation(program, attr_pos, "pos");
   glBindAttribLocation(program, attr_a_texcoord, "a_texcoord");
   glLinkProgram(program);  /* needed to put attribs into effect */

   u_matrix = glGetUniformLocation(program, "modelviewProjection");
   printf("Uniform modelviewProjection at %d\n", u_matrix);
   printf("Attrib pos at %d\n", attr_pos);
   printf("Attrib color at %d\n", attr_color);
   printf("Attrib a_texcoord at %d\n", attr_a_texcoord);
}
//Function Ends
/*********************************************************************************************************************************************************/


//Function Statrts
/*********************************************************************************************************************************************************/
static void queue_V4L2_buffer(struct stream_buf *vide0buf, struct EGLImgpack *b)
{
	struct v4l2_buffer vb;
	int ret;
	memset(&vb, 0, sizeof vb);
	vb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vb.memory = V4L2_MEMORY_MMAP;
	vb.index = b->index;    
	vb.m.fd = b->dbuf_fd;   
	ret = ioctl(vide0buf->fd, VIDIOC_QBUF, &vb);
	if (ret < 0)
   		printf("%s %d %d\n", __FUNCTION__, __LINE__, ret);
}                               
//Function Ends
/*********************************************************************************************************************************************************/


//Function Statrts
/*********************************************************************************************************************************************************/
static int dequeue_V4L2_buffer(struct stream_buf *vide0buf)
{
	struct v4l2_buffer vb;
	int ret;
	memset(&vb, 0, sizeof vb);
	vb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vb.memory = V4L2_MEMORY_MMAP;
	ret = ioctl(vide0buf->fd, VIDIOC_DQBUF, &vb);
	if (ret < 0)
   		printf("VIDIOC_DQBUF IOCTL return BAD");
	return vb.index;
}
//Function Ends
/*********************************************************************************************************************************************************/


//Function Statrts
/*********************************************************************************************************************************************************/
static struct stream_buf *PrepareV4L2buf(const char *dev_name)
{
	int fd, ret;
	struct v4l2_capability caps;
	struct v4l2_format fmt;
	struct v4l2_requestbuffers rqbufs;
	struct v4l2_exportbuffer ebuf;
	struct v4l2_buffer vbuf;
	unsigned int i, bpp;
	struct stream_buf *vide0buf;

	fd = open(dev_name, O_RDWR);
	if (fd < 0)
   		printf("Open /dev/video0 BAD\n");
	vide0buf = malloc(sizeof(*vide0buf));
	vide0buf->fd = fd;

	memset(&caps, 0, sizeof(caps));
        ret = ioctl(fd, VIDIOC_QUERYCAP, &caps);
	if (ret < 0)
   		printf("VIDIOC_QUERYCAP IOCTL return BAD\n");

	if (!(caps.capabilities & V4L2_CAP_VIDEO_CAPTURE))
   		printf("%s %d %d\n", __FUNCTION__, __LINE__, caps.capabilities);

	memset(&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
	if (ret < 0)
   		printf("%s %d %d\n", __FUNCTION__, __LINE__, ret);

#if TEX_RAW
	bpp = 3;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
	fmt.fmt.pix.bytesperline = tex_width * bpp;;
	fmt.fmt.pix.sizeimage = tex_height * fmt.fmt.pix.bytesperline;;
#else
#if RGB
	bpp = 4;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR32;
	fmt.fmt.pix.bytesperline = tex_width * bpp;
	fmt.fmt.pix.sizeimage = tex_height * fmt.fmt.pix.bytesperline;;
#else
	bpp = 4;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.bytesperline = tex_width * bpp;
	fmt.fmt.pix.sizeimage = tex_height * fmt.fmt.pix.bytesperline;
#endif
#endif
	fmt.fmt.pix.width = tex_width;
	fmt.fmt.pix.height = tex_height;

        ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
	if (ret < 0)
   		printf("VIDIOC_S_FMT IOCTL return BAD\n");

	tex_width = fmt.fmt.pix.width;
	tex_height = fmt.fmt.pix.height;
	fmt.fmt.pix.bytesperline = tex_width * bpp;

        ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
	if (ret < 0)
   		printf("%s %d %d\n", __FUNCTION__, __LINE__, ret);

	if (TEX_WIDTH != fmt.fmt.pix.width ||
			TEX_HEIGHT != fmt.fmt.pix.height ||
			TEX_WIDTH * bpp != fmt.fmt.pix.bytesperline) {
   		printf("%s %d format different from requested\n",
				__FUNCTION__, __LINE__);
   		printf("%s %d width: %d, height: %d, pitch: %d\n",
				__FUNCTION__, __LINE__,
				fmt.fmt.pix.width, fmt.fmt.pix.height,
				fmt.fmt.pix.bytesperline);
	}

	memset(&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
	if (ret < 0)
   		printf("%s %d %d\n", __FUNCTION__, __LINE__, ret);

	memset(&rqbufs, 0, sizeof(rqbufs));
	rqbufs.count = BUF_SIZE;
	rqbufs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;;
	rqbufs.memory = V4L2_MEMORY_MMAP;

        ret = ioctl(fd, VIDIOC_REQBUFS, &rqbufs);
	if (ret < 0)
   		printf("%s %d %d\n", __FUNCTION__, __LINE__, ret);

	for (i = 0; i < BUF_SIZE; i++) {
		memset(&ebuf, 0, sizeof(ebuf));
		ebuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		ebuf.index = i;
		ret = ioctl(fd, VIDIOC_EXPBUF, &ebuf);
		if (ret < 0)
   			printf("VIDIOC_EXPBUF IOCTL return BAD\n");

		vide0buf->EGLcushion[i].index = i;
		vide0buf->EGLcushion[i].dbuf_fd = ebuf.fd;

		memset(&vbuf, 0, sizeof(vbuf));
		vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		vbuf.memory = V4L2_MEMORY_MMAP;
		vbuf.index = i;
		vbuf.m.fd = vide0buf->EGLcushion[i].dbuf_fd;
		ret = ioctl(fd, VIDIOC_QBUF, &vbuf);
		if (ret < 0)
   			printf("VIDIOC_QBUF IOCTL return BAD\n");

		memset(&vbuf, 0, sizeof(vbuf));
		vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		vbuf.memory = V4L2_MEMORY_MMAP;
		vbuf.index = i;
		vbuf.m.fd = vide0buf->EGLcushion[i].dbuf_fd;
		ret = ioctl(fd, VIDIOC_QUERYBUF, &vbuf);
		if (ret < 0)
   			printf("QUERY BUFFER IOCTL return BAD\n");

		vide0buf->EGLcushion[i].buf = mmap(0, vbuf.length, PROT_READ | PROT_WRITE, MAP_SHARED, vide0buf->fd, vbuf.m.offset);
		vide0buf->EGLcushion[i].length = vbuf.length;
		if (vide0buf->EGLcushion[i].buf == MAP_FAILED)
   			printf("MMAP FAILED\n");
	}
	
	return vide0buf;
}
//Function Ends
/*********************************************************************************************************************************************************/



//Function Statrts
/*********************************************************************************************************************************************************/
static void
prepareStream(struct stream_buf *vide0buf)
{
	int ret;
	unsigned int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	struct v4l2_control ctrl;

        ret = ioctl(vide0buf->fd, VIDIOC_STREAMON, &type);
	if (ret < 0)
   		printf("STREAM_ON Failed\n");

}
//Function Ends
/*********************************************************************************************************************************************************/


//Function Statrts
/*********************************************************************************************************************************************************/
static void
prepareEGLImg(struct stream_buf *vide0buf, EGLDisplay egl_dpy)
{
   typedef void (*proc)();
   unsigned int i, j, k;
   int fd, size;
   glClearColor(1.0, 0.2, 0.1, 0.5);
   for (i = 0; i < BUF_SIZE; i++) {
   	EGLint egl_img_attr[] = { EGL_IMAGE_PRESERVED_KHR, EGL_TRUE,
		EGL_DMA_BUF_PLANE0_FD_EXT, vide0buf->EGLcushion[i].dbuf_fd,
		EGL_DMA_BUF_PLANE0_OFFSET_EXT, 0,
		EGL_DMA_BUF_PLANE0_PITCH_EXT, tex_width * 2,
		EGL_WIDTH, tex_width,
		EGL_HEIGHT, tex_height,
		EGL_LINUX_DRM_FOURCC_EXT, DRM_FORMAT_YUYV,
		EGL_YUV_COLOR_SPACE_HINT_EXT, EGL_ITU_REC601_EXT,
		EGL_SAMPLE_RANGE_HINT_EXT, EGL_YUV_FULL_RANGE_EXT,
		EGL_NONE, EGL_NONE };
   		vide0buf->EGLcushion[i].egl_img = eglCreateImageKHR(egl_dpy, EGL_NO_CONTEXT, EGL_LINUX_DMA_BUF_EXT, (EGLClientBuffer)0, egl_img_attr);
   }
   glGenTextures(1, &tex);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_EXTERNAL_OES, tex);
   glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   create_shaders_texture();
}

//Function Ends
/*********************************************************************************************************************************************************/


//Function Statrts
/*********************************************************************************************************************************************************/
static void
make_fbdev_window(Display *x_dpy, EGLDisplay egl_dpy,
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
		printf("EGL %d.%d\n", major, minor);
		printf("EGL_VENDOR: %s\n", eglQueryString(egl_dpy, EGL_VENDOR));
		printf("EGL_VERSION: %s\n", eglQueryString(egl_dpy, EGL_VERSION));
		printf("EGL_EXTENSIONS: %s\n", eglQueryString(egl_dpy, EGL_EXTENSIONS));
		printf("EGL_CLIENT_APIS: %s\n",
				eglQueryString(egl_dpy, EGL_CLIENT_APIS));
	}

	attrib[i++] = EGL_RENDERABLE_TYPE;
	attrib[i++] = EGL_OPENGL_ES2_BIT;
	attrib[i++] = EGL_BUFFER_SIZE;
	attrib[i++] = vinfo.bits_per_pixel;
	attrib[i++] = EGL_NONE;

	if (!eglChooseConfig(egl_dpy, attrib, &conf, 1, &num_configs) || !num_configs)
		printf("failed to choose a config");

	if (egl_verbose) {
		EGLint tmp;
		eglGetConfigAttrib(egl_dpy, conf, EGL_BUFFER_SIZE, &tmp);
		printf("buffer size: %d\n", tmp);
		eglGetConfigAttrib(egl_dpy, conf, EGL_RED_SIZE, &tmp);
		printf("red size: %d\n", tmp);
		eglGetConfigAttrib(egl_dpy, conf, EGL_GREEN_SIZE, &tmp);
		printf("green size: %d\n", tmp);
		eglGetConfigAttrib(egl_dpy, conf, EGL_BLUE_SIZE, &tmp);
		printf("blue size: %d\n", tmp);
		eglGetConfigAttrib(egl_dpy, conf, EGL_CONFIG_ID, &tmp);
		printf("config id: %d\n", tmp);
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
static void
event_loop(struct stream_buf *vide0buf, EGLDisplay egl_dpy, EGLSurface egl_surf)
{
	int ret, buf_id;
	struct pollfd fd = { .fd = vide0buf->fd, .events = POLLIN };

	while ((ret = poll(&fd, 1, 5000)) > 0) {
		if (fd.revents & POLLIN) {
			buf_id = dequeue_V4L2_buffer(vide0buf);
			draw_texture(&vide0buf->EGLcushion[buf_id]);
			eglSwapBuffers(egl_dpy, egl_surf);
			queue_V4L2_buffer(vide0buf, &vide0buf->EGLcushion[buf_id]);
		}
	}
}
//Function Ends
/*********************************************************************************************************************************************************/


//Function Statrts
/*********************************************************************************************************************************************************/
static void
usage(void)
{
   printf("Usage:\n");
   printf("  -display <displayname>  set the display to run on\n");
   printf("  -info                   display OpenGL renderer info\n");
}
//Function Ends
/*********************************************************************************************************************************************************/


//Function Statrts
/*********************************************************************************************************************************************************/
int
main(int argc, char *argv[])
{
   const int winWidth = 3840, winHeight = 2160;
   Display *x_dpy;
   Window win;
   EGLSurface egl_surf;
   EGLContext egl_ctx;
   EGLDisplay egl_dpy;
   char *dpyName = NULL;
   GLboolean printInfo = GL_TRUE;
   EGLint egl_major, egl_minor;
   int i;
   const char *s;
   struct stream_buf *vide0buf;

   vide0buf = PrepareV4L2buf("/dev/video0");

   x_dpy = EGL_DEFAULT_DISPLAY;
   egl_dpy = eglGetDisplay(x_dpy);

   if (!egl_dpy) {
      printf("Error: eglGetDisplay() failed\n");
      return -1;
   }

   if (!eglInitialize(egl_dpy, &egl_major, &egl_minor)) {
      printf("Error: eglInitialize() failed\n");
      return -1;
   }

   s = eglQueryString(egl_dpy, EGL_VERSION);
   printf("EGL_VERSION = %s\n", s);
   s = eglQueryString(egl_dpy, EGL_VENDOR);
   printf("EGL_VENDOR = %s\n", s);
   s = eglQueryString(egl_dpy, EGL_EXTENSIONS);
   printf("EGL_EXTENSIONS = %s\n", s);
   s = eglQueryString(egl_dpy, EGL_CLIENT_APIS);
   printf("EGL_CLIENT_APIS = %s\n", s);
   

   make_fbdev_window(x_dpy, egl_dpy,
                 "OpenGL ES 2.x tri", 0, 0, winWidth, winHeight,
                  &egl_ctx, &egl_surf);

   if (!eglMakeCurrent(egl_dpy, egl_surf, egl_surf, egl_ctx)) {
      printf("Error: eglMakeCurrent() failed\n");
      return -1;
   }

   if (printInfo) {
      printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
      printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
      printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
      printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
   }

   prepareEGLImg(vide0buf, egl_dpy);
   reshape(winWidth, winHeight);
   prepareStream(vide0buf);
   event_loop(vide0buf, egl_dpy, egl_surf);
   eglDestroyContext(egl_dpy, egl_ctx);
   eglDestroySurface(egl_dpy, egl_surf);
   eglTerminate(egl_dpy);

   return 0;
}
//Function Ends
/*********************************************************************************************************************************************************/

