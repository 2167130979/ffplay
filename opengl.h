#include <GL\glew.h>
#include <stdio.h>
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma warning( disable : 4996)

HGLRC    m_hRC;
HDC      m_hDC;
GLuint id_y, id_u, id_v;
GLuint textureUniformY, textureUniformU, textureUniformV;
#define ATTRIB_VERTEX 3
#define ATTRIB_TEXTURE 4
void InitShaders();

void Init(HWND hWnd)
{
	m_hDC = GetDC(hWnd);
	static PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };
	int nPixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	SetPixelFormat(m_hDC, nPixelFormat, &pfd);
	m_hRC = wglCreateContext(m_hDC);
	wglMakeCurrent(m_hDC, m_hRC);
	InitShaders();
}

void display(void *y, void *u, void *v, int width, int height)
{
	//Y
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id_y);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, y);
	glUniform1i(textureUniformY, 0);
	//U
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, id_u);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, u);
	glUniform1i(textureUniformU, 1);
	//V
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, id_v);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, v);
	glUniform1i(textureUniformV, 2);
	//draw
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	SwapBuffers(m_hDC);
}

void InitShaders()
{
	glewInit();
	GLuint program;
	GLint vertCompiled, fragCompiled, linked;

	const char *vs = "attribute vec4 vertexIn;attribute vec2 textureIn;varying vec2 textureOut;void main(void){gl_Position = vertexIn; textureOut = textureIn;}";
	const char *fs = "varying vec2 textureOut;uniform sampler2D tex_y,tex_u,tex_v;void main(void){vec3 yuv,rgb;yuv.x = texture2D(tex_y, textureOut).r;yuv.y = texture2D(tex_u, textureOut).r - 0.5;yuv.z = texture2D(tex_v, textureOut).r - 0.5;rgb = mat3( 1,1,1,0,-0.39465,2.03211,1.13983,-0.58060, 0)*yuv; gl_FragColor = vec4(rgb, 1);}";
	GLint v = glCreateShader(GL_VERTEX_SHADER);
	GLint f = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(v, 1, &vs, NULL);
	glShaderSource(f, 1, &fs, NULL);
	//Shader: step3
	glCompileShader(v);
	//Debug
	glGetShaderiv(v, GL_COMPILE_STATUS, &vertCompiled);
	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &fragCompiled);

	//Program: Step1
	program = glCreateProgram();
	//Program: Step2
	glAttachShader(program, v);
	glAttachShader(program, f);

	glBindAttribLocation(program, ATTRIB_VERTEX, "vertexIn");
	glBindAttribLocation(program, ATTRIB_TEXTURE, "textureIn");
	//Program: Step3
	glLinkProgram(program);
	//Debug
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	//Program: Step4
	glUseProgram(program);


	//Get Uniform Variables Location
	textureUniformY = glGetUniformLocation(program, "tex_y");
	textureUniformU = glGetUniformLocation(program, "tex_u");
	textureUniformV = glGetUniformLocation(program, "tex_v");


	static const GLfloat vertexVertices[] = {
		-1.0f, -1.0f,
		1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, 1.0f,
	};

	static const GLfloat textureVertices[] = {
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
	};
	//Set Arrays
	glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, vertexVertices);
	//Enable it
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, textureVertices);
	glEnableVertexAttribArray(ATTRIB_TEXTURE);


	//Init Texture
	glGenTextures(1, &id_y);
	glBindTexture(GL_TEXTURE_2D, id_y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &id_u);
	glBindTexture(GL_TEXTURE_2D, id_u);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenTextures(1, &id_v);
	glBindTexture(GL_TEXTURE_2D, id_v);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}