#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <glew.h>
#include <GL/glut.h>
#include <chrono>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

bool example = false;

GLuint v, f;
GLuint p;

GLuint VAO, VBO;

// Variáveis globais
unsigned int object;
vector<vector<float>> vertices_obj;
vector<vector<int>> faces;
vector<vector<float>> normals;
vector<vector<float>> textureBounds;
float translateX, translateY, translateZ;
float rotationX, rotationY, rotationZ;
float rotVel = 3.5;
bool light1, light2, light3;
float scale = 1;
float ang_passo = 0.1;
float angulo_x = 0;
float angulo_y = 0;
float angulo_z = 0;
float pos_x = 0;
float pos_y = 0;
float pos_z = 0;

float* vertices;
vector<float> vertices_vector;
int bufferArraySize;

static unsigned int texturas[2];

float intensidadeLuzAmbiente = 0.1;
float intensidadeLuzDifusa = 0.9;
float corLuz[3] = { 1, 1, 1 };
float posLuz[3] = { 0, 1, 0.25 };


struct BitMapFile {
	int sizeX;
	int sizeY;
	unsigned char* data;
};

std::vector<std::string> split(std::string, char);

// Carregando arquivo OBJ
void loadObj(string fname) {
	int read;
	float x, y, z;
	ifstream arquivo(fname);
	if (!arquivo.is_open()) {
		cout << "arquivo nao encontrado";
		exit(1);
	}
	else {
		string tipo;
		while (arquivo >> tipo) {

			if (tipo == "v") {
				vector<float> vertice;
				float x, y, z;
				arquivo >> x >> y >> z;
				vertice.push_back(x);
				vertice.push_back(y);
				vertice.push_back(z);
				vertices_obj.push_back(vertice);
			}

			if (tipo == "vt") {
				vector<float> textura;
				float x, y;
				arquivo >> x >> y;
				textura.push_back(x);
				textura.push_back(y);
				textureBounds.push_back(textura);
			}

			if (tipo == "vn") {
				vector<float> normal;
				float x, y, z;
				arquivo >> x >> y >> z;
				normal.push_back(x);
				normal.push_back(y);
				normal.push_back(z);
				normals.push_back(normal);
			}

			if (tipo == "f") {
				vector<int> face;
				string x, y, z;
				arquivo >> x >> y >> z;
				vector<vector<string>> data;
				data.push_back(split(x, '/'));
				data.push_back(split(y, '/'));
				data.push_back(split(z, '/'));
				for (auto d : data) {
					for (auto e : d) {
						if (e.empty()) e = "0";
						int val = stoi(e);
						if (val > 0) val--;
						face.push_back(val);
					}
				}
				faces.push_back(face);
			}
		}
	}

	arquivo.close();

	// vertice              textura       normal
	// 0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
	// 0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
	// 0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
	for (int i = 0; i < faces.size(); i++) {
		vector<int> face = faces[i];

		// vertice 1
		vertices_vector.push_back(vertices_obj[face[0]][0]);
		vertices_vector.push_back(vertices_obj[face[0]][1]);
		vertices_vector.push_back(vertices_obj[face[0]][2]);

		vertices_vector.push_back(textureBounds[face[1]][0]);
		vertices_vector.push_back(textureBounds[face[1]][1]);

		vertices_vector.push_back(normals[face[2]][0]);
		vertices_vector.push_back(normals[face[2]][1]);
		vertices_vector.push_back(normals[face[2]][2]);

		// vertice 2
		vertices_vector.push_back(vertices_obj[face[3]][0]);
		vertices_vector.push_back(vertices_obj[face[3]][1]);
		vertices_vector.push_back(vertices_obj[face[3]][2]);

		vertices_vector.push_back(textureBounds[face[4]][0]);
		vertices_vector.push_back(textureBounds[face[4]][1]);

		vertices_vector.push_back(normals[face[5]][0]);
		vertices_vector.push_back(normals[face[5]][1]);
		vertices_vector.push_back(normals[face[5]][2]);

		// vertice 3
		vertices_vector.push_back(vertices_obj[face[6]][0]);
		vertices_vector.push_back(vertices_obj[face[6]][1]);
		vertices_vector.push_back(vertices_obj[face[6]][2]);

		vertices_vector.push_back(textureBounds[face[7]][0]);
		vertices_vector.push_back(textureBounds[face[7]][1]);

		vertices_vector.push_back(normals[face[8]][0]);
		vertices_vector.push_back(normals[face[8]][1]);
		vertices_vector.push_back(normals[face[8]][2]);
	}
	bufferArraySize = vertices_vector.size();
	vertices = &vertices_vector[0];

}

void loadExample() {
	// mapear as informações, as faces
	//  vertice              textura       normal
	bufferArraySize = 32;
	vertices = new float[bufferArraySize] {
		0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
	   -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
	   -0.5f,  0.5f, 0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
	};
}

// Funciona somente com bmp de 24 bits
BitMapFile* getBMPData(string filename) {
	BitMapFile* bmp = new BitMapFile;
	unsigned int size, offset, headerSize;

	// Ler o arquivo de entrada
	ifstream infile(filename.c_str(), ios::binary);

	// Pegar o ponto inicial de leitura
	infile.seekg(10);
	infile.read((char*)&offset, 4);

	// Pegar o tamanho do cabeçalho do bmp.
	infile.read((char*)&headerSize, 4);

	// Pegar a altura e largura da imagem no cabeçalho do bmp.
	infile.seekg(18);
	infile.read((char*)&bmp->sizeX, 4);
	infile.read((char*)&bmp->sizeY, 4);

	// Alocar o buffer para a imagem.
	size = bmp->sizeX * bmp->sizeY * 24;
	bmp->data = new unsigned char[size];

	// Ler a informação da imagem.
	infile.seekg(offset);
	infile.read((char*)bmp->data, size);

	// Reverte a cor de bgr para rgb
	int temp;
	for (int i = 0; i < size; i += 3) {
		temp = bmp->data[i];
		bmp->data[i] = bmp->data[i + 2];
		bmp->data[i + 2] = temp;
	}

	return bmp;
}

void loadExternalTextures(std::string nome_arquivo, int id_textura) {
	BitMapFile* image[1];

	image[0] = getBMPData(nome_arquivo);

	glBindTexture(GL_TEXTURE_2D, texturas[id_textura]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0]->sizeX, image[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image[0]->data);
}

char* readStringFromFile(char* fn) {
	FILE* fp;
	char* content = NULL;
	int count = 0;

	if (fn != NULL) {
		fopen_s(&fp, fn, "rt");
		if (fp != NULL) {
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char*)malloc(sizeof(char) * (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

void setShaders() {
	char* vs = NULL, * fs = NULL, * fs2 = NULL;

	glewInit();

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	char vertex_shader[] = "simples.vert";
	char fragment_shader[] = "simples.frag";
	vs = readStringFromFile(vertex_shader);
	fs = readStringFromFile(fragment_shader);

	const char* vv = vs;
	const char* ff = fs;

	glShaderSource(v, 1, &vv, NULL);
	glShaderSource(f, 1, &ff, NULL);

	free(vs); free(fs);

	glCompileShader(v);
	glCompileShader(f);

	int result;
	glGetShaderiv(v, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int tam;
		glGetShaderiv(v, GL_INFO_LOG_LENGTH, &tam);
		char* mensagem = new char[tam];
		glGetShaderInfoLog(v, tam, &tam, mensagem);
		std::cout << mensagem << std::endl;
	}

	glGetShaderiv(f, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int tam;
		glGetShaderiv(f, GL_INFO_LOG_LENGTH, &tam);
		char* mensagem = new char[tam];
		glGetShaderInfoLog(f, tam, &tam, mensagem);
		std::cout << mensagem << std::endl;
	}

	p = glCreateProgram();
	glAttachShader(p, v);
	glAttachShader(p, f);

	glLinkProgram(p);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bufferArraySize * sizeof(float), vertices, GL_STATIC_DRAW);
	//                  pos  tam                    tam total                 pos apos tam
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glUseProgram(p);

}

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glPushMatrix();

	int id_pos_x = glGetUniformLocation(p, "pos_x");
	glUniform1f(id_pos_x, pos_x);

	int id_pos_y = glGetUniformLocation(p, "pos_y");
	glUniform1f(id_pos_y, pos_y);

	int id_pos_z = glGetUniformLocation(p, "pos_z");
	glUniform1f(id_pos_z, pos_z);

	int id_angulo_x = glGetUniformLocation(p, "angulo_x");
	glUniform1f(id_angulo_x, angulo_x);

	int id_angulo_y = glGetUniformLocation(p, "angulo_y");
	glUniform1f(id_angulo_y, angulo_y);

	int id_angulo_z = glGetUniformLocation(p, "angulo_z");
	glUniform1f(id_angulo_z, angulo_z);

	int id_scale = glGetUniformLocation(p, "scale");
	glUniform1f(id_scale, scale);

	int id_int_luz_amb = glGetUniformLocation(p, "luz_int_amb");
	glUniform1f(id_int_luz_amb, intensidadeLuzAmbiente);

	int id_int_luz_dif = glGetUniformLocation(p, "luz_int_dif");
	glUniform1f(id_int_luz_dif, intensidadeLuzDifusa);

	int id_cor_luz = glGetUniformLocation(p, "cor_luz");
	glUniform3f(id_cor_luz, corLuz[0], corLuz[1], corLuz[2]);

	int id_pos_luz = glGetUniformLocation(p, "pos_luz");
	glUniform3f(id_pos_luz, posLuz[0], posLuz[1], posLuz[2]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texturas[0]);
	int sp_texture = glGetUniformLocation(p, "textura");
	glUniform1i(sp_texture, 0);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, texturas[1]);
	int sp_texture_normal = glGetUniformLocation(p, "textura_normal");
	glUniform1i(sp_texture_normal, 1);


	glBindTexture(GL_TEXTURE_2D, texturas[1]);
	glBindVertexArray(VAO);
	// obj normal vai ser gl_triangle e vai de 0 até a quantidade de vertices
	if (example) {
		glDrawArrays(GL_QUADS, 0, 4);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, faces.size() * 3);
	}

	glPopMatrix();

	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}

void key(unsigned char k, int x, int y) {
	if (k == 'u')
		intensidadeLuzAmbiente -= 0.05;
	else if (k == 'o')
		intensidadeLuzAmbiente += 0.05;

	if (k == 'j')
		posLuz[0] -= 0.05;
	else if (k == 'l')
		posLuz[0] += 0.05;

	if (k == 'i')
		posLuz[1] += 0.05;
	else if (k == 'k')
		posLuz[1] -= 0.05;

	if (k == 'q')
		pos_z -= 0.05;
	else if (k == 'e')
		pos_z += 0.05;

	if (k == 's')
		pos_y -= 0.05;
	else if (k == 'w')
		pos_y += 0.05;

	if (k == 'd')
		pos_x += 0.05;
	else if (k == 'a')
		pos_x -= 0.05;

	switch (k) {
	case 55:
		// Rotacionar para baixo
		angulo_x += ang_passo;
		break;
	case 57:
		// Rotacionar para cima
		angulo_x -= ang_passo;
		break;
	case 52:
		// Rotacionar para esquerda
		angulo_y += ang_passo;
		break;
	case 54:
		// Rotacionar para direita
		angulo_y -= ang_passo;
		break;
	case 49:
		// Rotacionar eixo z
		angulo_z += ang_passo;
		break;
	case 51:
		// Rotacionar eixo z
		angulo_z -= ang_passo;
		break;
	case 53:
		angulo_x += rand() % 30 - 15;
		angulo_y += rand() % 30 - 15;
		angulo_z += rand() % 20 - 10;
		break;
	}
}

void keyboard_special(int key, int x, int y) {
	std::cout << "Special Key:" << key << std::endl;
	switch (key) {
		// aumentar escala
	case GLUT_KEY_PAGE_UP:
		scale += 0.1;
		break;
		// diminuir escala
	case GLUT_KEY_PAGE_DOWN:
		if (scale > 0.2)
			scale -= 0.1;
		break;
	}
}

void reshape(int width, int height) {
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (float)640 / (float)480, 0.1f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void setupRC() {
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat diffuseLight[] = { 0.7, 0.7, 0.7, 1.0 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glEnable(GL_LIGHT0);

	GLfloat ambientLight[] = { 0.05, 0.05, 0.05, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);

	glEnable(GL_LIGHTING);
}

void init() {
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	setupRC();

	glGenTextures(2, texturas);
	loadExternalTextures("Textures/brickwall.bmp", 0);
	loadExternalTextures("Textures/brickwall_normal.bmp", 1);
	glEnable(GL_TEXTURE_2D);

	//comente essa linha para desabilitar os shaders
	setShaders();
}

std::vector<std::string> split(std::string string, char splitChar) {
	auto strings = std::vector<std::string>();
	std::string currentString = "";
	for (char currentChar : string) {
		if (currentChar == splitChar) {
			strings.push_back(currentString);
			currentString = "";
			continue;
		}
		currentString += currentChar;
	}
	if (!currentString.empty()) {
		strings.push_back(currentString);
	}
	return strings;
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Trabalho M2/M3");

	if (example) {
		loadExample();
	}
	else {
		loadObj("objs/cubo.obj");
	}

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	glutSpecialFunc(keyboard_special);

	glutMainLoop();

	return 0;
}