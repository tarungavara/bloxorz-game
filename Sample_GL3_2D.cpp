#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <map>
#include <string>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
using namespace std;

struct VAO {
  GLuint VertexArrayID;
  GLuint VertexBuffer;
  GLuint ColorBuffer;
  GLenum PrimitiveMode;
  GLenum FillMode;
  int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices {
  glm::mat4 projectionO, projectionP;
  glm::mat4 model;
  glm::mat4 view;
  GLuint MatrixID;
} Matrices;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
GLuint fontVAO, fontVBO;

void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

GLuint programID, attribute_coord;
GLfloat fov = 90.0f;
double last_update_time2, last_update_time, current_time, MouseLeftClickX, MouseLeftClickY;
float CameraXOffset=0, CameraYOffset=0, CameraZOffset=0;
int CameraBackupX=0, CameraBackupY=0, CameraBackupZ=0;
int BoxFlagUp=0, BoxFlagDown=0, BoxFlagLeft=0, BoxFlagRight=0;
int proj_type=1;
glm::vec3 boxRotateAboutVec (0.0f,0.0f,1.0f);
int ShiftFlag=0;
float boxScaleX=1.0f, boxScaleY=1.0f, boxScaleZ=1.0f;
int BoxIsStanding = 1;
int upx=0, upy=0, upz=0, tx=0, ty=0, tz=0;
int Info[200][200][5]; //Info[Level][Tile][Value]
int tileIndex, tileIndex2, TileUnderBox;
int Level=0, Level2, Moves=0, MovesBackup=2500;
int BoxStart[5][3];
float BoxOffsetX=0, BoxOffsetY=0, BoxOffsetZ=0, boxDegree=0;
int BoxViewFlag=0, Max_Level=2, FollowCam=0;
GLfloat th = -0.5;
int CamKeyA=0, CamKeyD=0, CamKeyW=0, CamKeyS=0, CamKeyZ=0, CamKeyX=0;
glm::vec3 cameraFront (0,-1,0);
int MouseLeftClick=0, MouseLeftInitial=0;
int GameEndingScreen=0, GameStartingScreen=1;
std::string message;
//Info[Level][tileIndex][0] = TypeOfTile  1->Normal 2->Fragile 3->Switch 4->EndTile 5->NoTile
//Info[Level][tileIndex][1] = BridgeTileLocation only if [0]==3 else default 300

// int infoLevel;
// Info[infoLevel][]

void createLevels()
{
  int i;

  // Level 0 - Just for Testing
  for (i=0; i<170; i++)
  {
    Info[0][i][0] = 1;
  }
  Info[0][26][0] = 2;
  Info[0][31][0] = 3;
  Info[0][31][1] = 33;
  Info[0][32][0] = 4;
  Info[0][33][0] = 5;
  BoxStart[0][0] = 0;
  BoxStart[0][1] = 2;
  BoxStart[0][2] = 32;

  // Level 1
  BoxStart[1][0] = -16;
  BoxStart[1][1] = -10;
  BoxStart[1][2] = 130;
  for (i=0; i<170; i++)
    Info[1][i][0] = 5;
  for (i=0; i<36; i++)
    Info[1][17*(i/6)+(i%6)][0] = 1;
  Info[1][18][0] = 3;
  Info[1][18][1] = 109;
  Info[1][106][0] = 1; Info[1][107][0] = 1; Info[1][108][0] = 1; Info[1][109][0] = 5; Info[1][110][0] = 1;
  Info[1][91][0] = 1; Info[1][92][0] = 2; Info[1][93][0] = 1; Info[1][94][0] = 1; Info[1][95][0] = 1;
  Info[1][96][0] = 1; Info[1][97][0] = 1; Info[1][98][0] = 1; Info[1][99][0] = 1; Info[1][111][0] = 1;
  Info[1][112][0] = 1; Info[1][113][0] = 1; Info[1][114][0] = 1; Info[1][115][0] = 1; Info[1][116][0] = 1;
  Info[1][74][0] = 1; Info[1][76][0] = 1; Info[1][77][0] = 1; Info[1][78][0] = 1; Info[1][79][0] = 1;
  Info[1][80][0] = 1; Info[1][81][0] = 1; Info[1][127][0] = 1; Info[1][128][0] = 1; Info[1][129][0] = 1;
  Info[1][130][0] = 4; Info[1][131][0] = 1; Info[1][132][0] = 1; Info[1][133][0] = 1; Info[1][145][0] = 1;
  Info[1][146][0] = 1; Info[1][147][0] = 1; Info[1][148][0] = 1; Info[1][149][0] = 1; Info[1][164][0] = 1;

  // Level 2
  BoxStart[2][0] = -14;
  BoxStart[2][1] = 0;
  BoxStart[2][2] = 65;
  for (i=0; i<170; i++)
    Info[2][i][0] = 5;
  Info[2][34][0] = 1; Info[2][35][0] = 1; Info[2][36][0] = 1; Info[2][37][0] = 1; Info[2][38][0] = 1;
  Info[2][51][0] = 1; Info[2][52][0] = 3; Info[2][53][0] = 1; Info[2][54][0] = 1; Info[2][55][0] = 1;
  Info[2][68][0] = 1; Info[2][69][0] = 1; Info[2][70][0] = 1; Info[2][71][0] = 1; Info[2][72][0] = 1;
  Info[2][85][0] = 1; Info[2][86][0] = 1; Info[2][87][0] = 1; Info[2][88][0] = 1; Info[2][89][0] = 1;
  Info[2][102][0] = 1; Info[2][103][0] = 1; Info[2][104][0] = 1; Info[2][105][0] = 1; Info[2][106][0] = 1;
  Info[2][119][0] = 1; Info[2][120][0] = 1; Info[2][121][0] = 1; Info[2][122][0] = 1; Info[2][123][0] = 1;

  Info[2][90][0] = 5; Info[2][61][0] = 5; Info[2][52][1] = 90; Info[2][91][1] = 61;
  // Info[2][][0] = 1; Info[2][][0] = 1; Info[2][][0] = 1; Info[2][][0] = 1; Info[2][][0] = 1;
  Info[2][23][0] = 2; Info[2][24][0] = 2; Info[2][25][0] = 2; Info[2][26][0] = 2;
  Info[2][40][0] = 2; Info[2][41][0] = 2; Info[2][42][0] = 2; Info[2][43][0] = 2;
  Info[2][57][0] = 2; Info[2][58][0] = 2; Info[2][59][0] = 2; Info[2][60][0] = 1;
  Info[2][74][0] = 2; Info[2][75][0] = 2; Info[2][76][0] = 2; Info[2][77][0] = 2;
  Info[2][91][0] = 3; Info[2][92][0] = 2; Info[2][93][0] = 2; Info[2][94][0] = 2;
  Info[2][108][0] = 2; Info[2][109][0] = 2; Info[2][110][0] = 2; Info[2][111][0] = 2;
  Info[2][125][0] = 2; Info[2][126][0] = 2; Info[2][127][0] = 2; Info[2][128][0] = 2;
  Info[2][142][0] = 2; Info[2][143][0] = 2; Info[2][144][0] = 2; Info[2][145][0] = 2;

  Info[2][45][0] = 1; Info[2][46][0] = 1; Info[2][47][0] = 1; Info[2][48][0] = 1; Info[2][49][0] = 1;
  Info[2][62][0] = 1; Info[2][63][0] = 1; Info[2][64][0] = 1; Info[2][65][0] = 4; Info[2][66][0] = 1;
  Info[2][79][0] = 1; Info[2][80][0] = 1; Info[2][81][0] = 1; Info[2][82][0] = 1; Info[2][83][0] = 1;
  Info[2][96][0] = 1; Info[2][97][0] = 1; Info[2][98][0] = 1; Info[2][99][0] = 1; Info[2][100][0] = 1;
  Info[2][113][0] = 1; Info[2][114][0] = 1; Info[2][115][0] = 1; Info[2][116][0] = 1; Info[2][117][0] = 1;
  Info[2][130][0] = 1; Info[2][131][0] = 1; Info[2][132][0] = 1; Info[2][133][0] = 1; Info[2][134][0] = 1;

}

/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path)
{
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  std::string VertexShaderCode;
  std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
  if(VertexShaderStream.is_open())
  {
    std::string Line = "";
    while(getline(VertexShaderStream, Line))
    VertexShaderCode += "\n" + Line;
    VertexShaderStream.close();
  }
  std::string FragmentShaderCode;
  std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
  if(FragmentShaderStream.is_open()){
    std::string Line = "";
    while(getline(FragmentShaderStream, Line))
    FragmentShaderCode += "\n" + Line;
    FragmentShaderStream.close();
  }
  GLint Result = GL_FALSE;
  int InfoLogLength;
  char const * VertexSourcePointer = VertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
  glCompileShader(VertexShaderID);
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  std::vector<char> VertexShaderErrorMessage(InfoLogLength);
  glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
  char const * FragmentSourcePointer = FragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
  glCompileShader(FragmentShaderID);
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
  glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragmentShaderID);
  glLinkProgram(ProgramID);
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
  glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);
  return ProgramID;
}

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window)
{
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}

void initGLEW(void)
{
  glewExperimental = GL_TRUE;
  if(glewInit()!=GLEW_OK){
    fprintf(stderr,"Glew failed to initialize : %s\n", glewGetErrorString(glewInit()));
  }
  if(!GLEW_VERSION_3_3)
  fprintf(stderr, "3.3 version not available\n");
}

struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
  struct VAO* vao = new struct VAO;
  vao->PrimitiveMode = primitive_mode;
  vao->NumVertices = numVertices;
  vao->FillMode = fill_mode;
  glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
  glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
  glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors
  glBindVertexArray (vao->VertexArrayID); // Bind the VAO
  glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices
  glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
  glVertexAttribPointer(
    0,                  // attribute 0. Vertices
    3,                  // size (x,y,z)
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );
  glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors
  glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
  glVertexAttribPointer(
    1,                  // attribute 1. Color
    3,                  // size (r,g,b)
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );
  return vao;
}

struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
  GLfloat* color_buffer_data = new GLfloat [3*numVertices];
  for (int i=0; i<numVertices; i++) {
    color_buffer_data [3*i] = red;
    color_buffer_data [3*i + 1] = green;
    color_buffer_data [3*i + 2] = blue;
  }
  return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}

void draw3DObject (struct VAO* vao)
{
  glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);
  glBindVertexArray (vao->VertexArrayID);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);
  glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
  glDisable(GL_BLEND);
}

// * Customizable functions *
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (action == GLFW_RELEASE)
  {
    if (key==GLFW_KEY_A)
      CamKeyA=0;
    if (key==GLFW_KEY_D)
      CamKeyD=0;
    if (key==GLFW_KEY_W)
      CamKeyW=0;
    if (key==GLFW_KEY_S)
      CamKeyS=0;
    if (key==GLFW_KEY_Z)
      CamKeyZ=0;
    if (key==GLFW_KEY_X)
      CamKeyX=0;
  }
  else if (action == GLFW_PRESS)
  {
    if (key == GLFW_KEY_ENTER)
    {
      GameStartingScreen = 0;
      GameEndingScreen = 0;
      Level = 0;
      glDisable(GL_BLEND);
      BoxOffsetX=BoxStart[Level][0]; BoxOffsetZ=BoxStart[Level][1];
    }
    if (BoxFlagUp == 0 && BoxFlagDown == 0 && BoxFlagLeft == 0 && BoxFlagRight == 0)
    {
      if (key==GLFW_KEY_UP)
        BoxFlagUp=1;
      if (key==GLFW_KEY_DOWN)
        BoxFlagDown=1;
      if (key==GLFW_KEY_LEFT)
        BoxFlagLeft=1;
      if (key==GLFW_KEY_RIGHT)
        BoxFlagRight=1;
    }
      if (key==GLFW_KEY_A)
        CamKeyA=1;
      if (key==GLFW_KEY_D)
        CamKeyD=1;
      if (key==GLFW_KEY_W)
        CamKeyW=1;
      if (key==GLFW_KEY_S)
        CamKeyS=1;
      if (key==GLFW_KEY_Z)
        CamKeyZ=1;
      if (key==GLFW_KEY_X)
        CamKeyX=1;
    // }
    // Top View
    if (key == GLFW_KEY_T)
    {
      if (proj_type==1)
      {
        BoxViewFlag=0;
        FollowCam=0;
        CameraXOffset=(-2+0);
        CameraYOffset=(-13+10);
        CameraZOffset=(-6+0);
        upy=-1;
        upz=-1;
        tx=-1;
        tz=-1;
        proj_type=1-proj_type;
      }
    }
    // Normal Tower View
    if (key == GLFW_KEY_N)
    {
      BoxViewFlag=0;
      FollowCam=0;
      proj_type = 1;
      CameraXOffset=0;
      CameraYOffset=0;
      CameraZOffset=0;
      upx=0;
      upy=0;
      upz=0;
      tx=0;
      ty=0;
      tz=0;
      fov = 90.0f;
    }
    // Block View
    if (key == GLFW_KEY_B)
    {
      BoxViewFlag=1;
      FollowCam=0;
    }
    // Follow Cam View
    if (key == GLFW_KEY_J)
    {
      FollowCam=1;
      BoxViewFlag=0;
    }
    if (key == GLFW_KEY_C)
      printf("2+%d 11+%d 8+%d\n", CameraXOffset, CameraYOffset, CameraZOffset);
    if (key == GLFW_KEY_V)
      printf("-1+%f 0+%f -1+%f %f\n", BoxOffsetX, BoxOffsetY, BoxOffsetZ, (17*(5+BoxOffsetZ/2)+(8+BoxOffsetX/2)));
    if (key == GLFW_KEY_F)
      printf("%d\n", TileUnderBox);
    if (key == GLFW_KEY_G)
      printf("%d\n", Moves);
  }
  else if (action == GLFW_REPEAT)
  {
    if (BoxFlagUp == 0 && BoxFlagDown == 0 && BoxFlagLeft == 0 && BoxFlagRight == 0)
    {
      if (key==GLFW_KEY_UP)
        BoxFlagUp=1;
      if (key==GLFW_KEY_DOWN)
        BoxFlagDown=1;
      if (key==GLFW_KEY_LEFT)
        BoxFlagLeft=1;
      if (key==GLFW_KEY_RIGHT)
        BoxFlagRight=1;
    }
  }
}

void keyboardChar (GLFWwindow* window, unsigned int key)
{
  switch (key) {
    case 'Q':
    case 'q':
    quit(window);
    break;
    case 'a':
    break;
    default:
    break;
  }
}

void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
  if (action == GLFW_RELEASE)
  {
    if (button == GLFW_MOUSE_BUTTON_LEFT)
      MouseLeftClick=0;
  }
  else if (action == GLFW_PRESS)
  {
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
      MouseLeftInitial = 1;
      MouseLeftClick = 1;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
      if ((proj_type==1) && (BoxViewFlag == 0) && (FollowCam == 0))
      {
        BoxViewFlag=0;
        CameraXOffset=(-2+0);
        CameraYOffset=(-13+10);
        CameraZOffset=(-6+0);
        upy=-1;
        upz=-1;
        tx=-1;
        tz=-1;
        proj_type=1-proj_type;
      }
      else
      {
        if ((BoxViewFlag == 0) && (FollowCam == 0))
          BoxViewFlag=1;
        else
        {
          if (FollowCam == 0)
          {
            FollowCam = 1;
          }
          else
          {
            BoxViewFlag=0;
            FollowCam=0;
            proj_type = 1;
            CameraXOffset=0;
            CameraYOffset=0;
            CameraZOffset=0;
            upx=0;
            upy=0;
            upz=0;
            tx=0;
            ty=0;
            tz=0;
            fov = 90.0f;
          }
        }
      }

    }
  }
  else if (action == GLFW_REPEAT)
  {

  }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  	fov -= 2*yoffset;
  if(fov < 1.0f)
  	fov = 1.0f;
  if(fov > 90.0f)
  	fov = 90.0f;
}

void reshapeWindow (GLFWwindow* window, int width, int height)
{
  int fbwidth=width, fbheight=height;
  glfwGetFramebufferSize(window, &fbwidth, &fbheight);
  glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);
  Matrices.projectionP = glm::perspective((GLfloat)(fov*M_PI/180), (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 100.0f);
  // Matrices.projectionO = glm::perspective(fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 100.0f);
  Matrices.projectionO = glm::ortho(-22.4f, 22.4f, -12.6f, 12.6f, 0.1f, 500.0f);
}

VAO *tile[6], *tile_mesh, *box, *box_mesh, *tile_base;

void createBox()
{
  GLfloat vertex_buffer_data [] = {
    2,0,0,//1
    0,0,0,//0
    0,0,2,//3
    0,0,2,//3
    0,0,0,//0
    0,4,0,//5
    2,0,0,//1
    0,0,0,//0
    0,4,0,//5

    2,0,2,//2
    2,4,2,//7
    0,4,2,//4
    0,4,2,//4
    2,4,2,//7
    2,4,0,//6
    2,0,2,//2
    2,4,2,//7
    2,4,0,//6

    2,0,0,//1
    2,0,2,//2
    0,0,2,//3
    0,4,2,//4
    0,4,0,//5
    2,4,0,//6
    2,0,2,//2
    0,0,2,//3
    0,4,2,//4
    2,0,2,//2
    2,0,0,//1
    2,4,0,//6
    0,0,2,//3
    0,4,2,//4
    0,4,0,//5
    2,0,0,//1
    2,4,0,//6
    0,4,0,//5
    2,0,0,//1
    2,0,2,//2
    0,0,2,//3
  };
  GLfloat color_buffer_data [] = {
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
    0.01171875f, 0.66015625f, 0.953125f,
  };
  box = create3DObject(GL_TRIANGLES, 12*3, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void createBoxMesh()
{
  GLfloat vertex_buffer_data [] = {
    2,0,0,//1
    0,0,0,//0
    0,0,2,//3
    0,0,2,//3
    0,0,0,//0
    0,4,0,//5
    2,0,0,//1
    0,0,0,//0
    0,4,0,//5

    2,0,2,//2
    2,4,2,//7
    0,4,2,//4
    0,4,2,//4
    2,4,2,//7
    2,4,0,//6
    2,0,2,//2
    2,4,2,//7
    2,4,0,//6

    2,0,0,//1
    2,0,2,//2
    0,0,2,//3
    0,4,2,//4
    0,4,0,//5
    2,4,0,//6
    2,0,2,//2
    0,0,2,//3
    0,4,2,//4
    2,0,2,//2
    2,0,0,//1
    2,4,0,//6
    0,0,2,//3
    0,4,2,//4
    0,4,0,//5
    2,0,0,//1
    2,4,0,//6
    0,4,0,//5
    2,0,0,//1
    2,0,2,//2
    0,0,2,//3
  };
  GLfloat color_buffer_data [] = {
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
  };
  box_mesh = create3DObject(GL_TRIANGLES, 12*3, vertex_buffer_data, color_buffer_data, GL_LINE);
}

void createTile()
{
  GLfloat vertex_buffer_data [] = {
    1, 0, 0,// vertex 0
    1, 0, 1,// vertex 1
    0, 0, 1,// vertex 2

    0, 0, 1,// vertex 2
    0, 0, 0,// vertex 3
    1, 0, 0,// vertex 0

    1, 0, 0,// vertex 0
    0, 0, 0,// vertex 3
    0, 0, -1,// vertex 4

    0, 0, -1,// vertex 4
    1, 0, -1,// vertex 5
    1, 0, 0,// vertex 0

    0, 0, -1,// vertex 4
    0, 0, 0,// vertex 3
    -1, 0, 0,// vertex 7

    -1, 0, 0,// vertex 7
    -1, 0, -1,// vertex 6
    0, 0, -1,// vertex 4

    -1, 0, 0,// vertex 7
    0, 0, 0,// vertex 3
    0, 0, 1,// vertex 2

    0, 0, 1,// vertex 2
    -1, 0, 1,// vertex 8
    -1, 0, 0,// vertex 7

    // -1,th,-1,
    // 1,th,-1,
    // 1,th,-1,
    //
    // 1,th,-1,
    // 1,th,1,
    // 1,th,1,
    //
    // 1,th,1,
    // -1,th,1,
    // -1,th,1,
    //
    // -1,th,1,
    // -1,th,-1,
    // -1,th,-1,

  };

  GLfloat color_buffer_data [] = {
    1,0.756862745,0.02745098,// vertex 0
    1,0.756862745,0.02745098,// vertex 1
    1,0.756862745,0.02745098,// vertex 2

    1,0.756862745,0.02745098,// vertex 2
    0.956862745,0.262745098,0.211764706,// vertex 3
    1,0.756862745,0.02745098,// vertex 0

    1,0.756862745,0.02745098,// vertex 0
    0.956862745,0.262745098,0.211764706,// vertex 3
    1,0.756862745,0.02745098,// vertex 4

    1,0.756862745,0.02745098,// vertex 4
    1,0.756862745,0.02745098,// vertex 5
    1,0.756862745,0.02745098,// vertex 0

    1,0.756862745,0.02745098,// vertex 4
    0.956862745,0.262745098,0.211764706,// vertex 3
    1,0.756862745,0.02745098,// vertex 7

    1,0.756862745,0.02745098,// vertex 7
    1,0.756862745,0.02745098,// vertex 6
    1,0.756862745,0.02745098,// vertex 4

    1,0.756862745,0.02745098,// vertex 7
    0.956862745,0.262745098,0.211764706,// vertex 3
    1,0.756862745,0.02745098,// vertex 2

    1,0.756862745,0.02745098,// vertex 2
    1,0.756862745,0.02745098,// vertex 8
    1,0.756862745,0.02745098,// vertex 7
  };
  tile[1] = create3DObject(GL_TRIANGLES, 8*3, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void createSwitchTile()
{
  GLfloat vertex_buffer_data [] = {
    1, 0, 0,// vertex 0
    1, 0, 1,// vertex 1
    0, 0, 1,// vertex 2

    0, 0, 1,// vertex 2
    0, 0, 0,// vertex 3
    1, 0, 0,// vertex 0

    1, 0, 0,// vertex 0
    0, 0, 0,// vertex 3
    0, 0, -1,// vertex 4

    0, 0, -1,// vertex 4
    1, 0, -1,// vertex 5
    1, 0, 0,// vertex 0

    0, 0, -1,// vertex 4
    0, 0, 0,// vertex 3
    -1, 0, 0,// vertex 7

    -1, 0, 0,// vertex 7
    -1, 0, -1,// vertex 6
    0, 0, -1,// vertex 4

    -1, 0, 0,// vertex 7
    0, 0, 0,// vertex 3
    0, 0, 1,// vertex 2

    0, 0, 1,// vertex 2
    -1, 0, 1,// vertex 8
    -1, 0, 0,// vertex 7
  };

  GLfloat color_buffer_data [] = {
    0.956862745,0.262745098,0.211764706,// vertex 0
    0.956862745,0.262745098,0.211764706,// vertex 1
    0.956862745,0.262745098,0.211764706,// vertex 2

    0.956862745,0.262745098,0.211764706,// vertex 2
    1,0.756862745,0.02745098,// vertex 3
    0.956862745,0.262745098,0.211764706,// vertex 0

    0.956862745,0.262745098,0.211764706,// vertex 0
    1,0.756862745,0.02745098,// vertex 3
    0.956862745,0.262745098,0.211764706,// vertex 4

    0.956862745,0.262745098,0.211764706,// vertex 4
    0.956862745,0.262745098,0.211764706,// vertex 5
    0.956862745,0.262745098,0.211764706,// vertex 0

    0.956862745,0.262745098,0.211764706,// vertex 4
    1,0.756862745,0.02745098,// vertex 3
    0.956862745,0.262745098,0.211764706,// vertex 7

    0.956862745,0.262745098,0.211764706,// vertex 7
    0.956862745,0.262745098,0.211764706,// vertex 6
    0.956862745,0.262745098,0.211764706,// vertex 4

    0.956862745,0.262745098,0.211764706,// vertex 7
    1,0.756862745,0.02745098,// vertex 3
    0.956862745,0.262745098,0.211764706,// vertex 2

    0.956862745,0.262745098,0.211764706,// vertex 2
    0.956862745,0.262745098,0.211764706,// vertex 8
    0.956862745,0.262745098,0.211764706,// vertex 7
  };
  tile[3] = create3DObject(GL_TRIANGLES, 8*3, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void createEndTile()
{
  GLfloat vertex_buffer_data [] = {
    1, 0, 0,// vertex 0
    1, 0, 1,// vertex 1
    0, 0, 1,// vertex 2

    0, 0, 1,// vertex 2
    0, 0, 0,// vertex 3
    1, 0, 0,// vertex 0

    1, 0, 0,// vertex 0
    0, 0, 0,// vertex 3
    0, 0, -1,// vertex 4

    0, 0, -1,// vertex 4
    1, 0, -1,// vertex 5
    1, 0, 0,// vertex 0

    0, 0, -1,// vertex 4
    0, 0, 0,// vertex 3
    -1, 0, 0,// vertex 7

    -1, 0, 0,// vertex 7
    -1, 0, -1,// vertex 6
    0, 0, -1,// vertex 4

    -1, 0, 0,// vertex 7
    0, 0, 0,// vertex 3
    0, 0, 1,// vertex 2

    0, 0, 1,// vertex 2
    -1, 0, 1,// vertex 8
    -1, 0, 0,// vertex 7
  };

  GLfloat color_buffer_data [] = {
    0.54296875, 0.76171875, 0.2890625,
    0.54296875, 0.76171875, 0.2890625,
    0.54296875, 0.76171875, 0.2890625,

    0.54296875, 0.76171875, 0.2890625,
    1,0.756862745,0.02745098,// vertex 3
    0.54296875, 0.76171875, 0.2890625,

    0.54296875, 0.76171875, 0.2890625,
    1,0.756862745,0.02745098,// vertex 3
    0.54296875, 0.76171875, 0.2890625,

    0.54296875, 0.76171875, 0.2890625,
    0.54296875, 0.76171875, 0.2890625,
    0.54296875, 0.76171875, 0.2890625,

    0.54296875, 0.76171875, 0.2890625,
    1,0.756862745,0.02745098,// vertex 3
    0.54296875, 0.76171875, 0.2890625,

    0.54296875, 0.76171875, 0.2890625,
    0.54296875, 0.76171875, 0.2890625,
    0.54296875, 0.76171875, 0.2890625,

    0.54296875, 0.76171875, 0.2890625,
    1,0.756862745,0.02745098,// vertex 3
    0.54296875, 0.76171875, 0.2890625,

    0.54296875, 0.76171875, 0.2890625,
    0.54296875, 0.76171875, 0.2890625,
    0.54296875, 0.76171875, 0.2890625,

  };
  tile[4] = create3DObject(GL_TRIANGLES, 8*3, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void createFragileTile()
{
  GLfloat vertex_buffer_data [] = {
    1, 0, 0,// vertex 0
    1, 0, 1,// vertex 1
    0, 0, 1,// vertex 2

    0, 0, 1,// vertex 2
    0, 0, 0,// vertex 3
    1, 0, 0,// vertex 0

    1, 0, 0,// vertex 0
    0, 0, 0,// vertex 3
    0, 0, -1,// vertex 4

    0, 0, -1,// vertex 4
    1, 0, -1,// vertex 5
    1, 0, 0,// vertex 0

    0, 0, -1,// vertex 4
    0, 0, 0,// vertex 3
    -1, 0, 0,// vertex 7

    -1, 0, 0,// vertex 7
    -1, 0, -1,// vertex 6
    0, 0, -1,// vertex 4

    -1, 0, 0,// vertex 7
    0, 0, 0,// vertex 3
    0, 0, 1,// vertex 2

    0, 0, 1,// vertex 2
    -1, 0, 1,// vertex 8
    -1, 0, 0,// vertex 7
  };

  GLfloat color_buffer_data [] = {
    0.275,0.38828125,0.44296875,
    0.275,0.38828125,0.44296875,
    0.275,0.38828125,0.44296875,
    0.275,0.38828125,0.44296875,
    0.956862745,0.262745098,0.211764706,// vertex 3
    0.275,0.38828125,0.44296875,
    0.275,0.38828125,0.44296875,
    0.956862745,0.262745098,0.211764706,// vertex 3
    0.275,0.38828125,0.44296875,
    0.275,0.38828125,0.44296875,
    0.275,0.38828125,0.44296875,
    0.275,0.38828125,0.44296875,
    0.275,0.38828125,0.44296875,
    0.956862745,0.262745098,0.211764706,// vertex 3
    0.275,0.38828125,0.44296875,
    0.275,0.38828125,0.44296875,
    0.275,0.38828125,0.44296875,
    0.275,0.38828125,0.44296875,
    0.275,0.38828125,0.44296875,
    0.956862745,0.262745098,0.211764706,// vertex 3
    0.275,0.38828125,0.44296875,
    0.275,0.38828125,0.44296875,
    0.275,0.38828125,0.44296875,
    0.275,0.38828125,0.44296875,
  };
  tile[2] = create3DObject(GL_TRIANGLES, 8*3, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void createTileMesh()
{
  GLfloat vertex_buffer_data [] = {
    1, 0, 0,// vertex 0
    1, 0, 1,// vertex 1
    0, 0, 1,// vertex 2

    0, 0, 1,// vertex 2
    0, 0, 0,// vertex 3
    1, 0, 0,// vertex 0

    1, 0, 0,// vertex 0
    0, 0, 0,// vertex 3
    0, 0, -1,// vertex 4

    0, 0, -1,// vertex 4
    1, 0, -1,// vertex 5
    1, 0, 0,// vertex 0

    0, 0, -1,// vertex 4
    0, 0, 0,// vertex 3
    -1, 0, 0,// vertex 7

    -1, 0, 0,// vertex 7
    -1, 0, -1,// vertex 6
    0, 0, -1,// vertex 4

    -1, 0, 0,// vertex 7
    0, 0, 0,// vertex 3
    0, 0, 1,// vertex 2

    0, 0, 1,// vertex 2
    -1, 0, 1,// vertex 8
    -1, 0, 0,// vertex 7
  };

  GLfloat color_buffer_data [] = {
    0,0,0,// vertex 0
    1,0.756862745,0.02745098,// vertex 1
    0,0,0,// vertex 2

    0,0,0,// vertex 2
    0.956862745,0.262745098,0.211764706,// vertex 3
    0,0,0,// vertex 0

    0,0,0,// vertex 0
    0.956862745,0.262745098,0.211764706,// vertex 3
    0,0,0,// vertex 4

    0,0,0,// vertex 4
    1,0.756862745,0.02745098,// vertex 5
    0,0,0,// vertex 0

    0,0,0,// vertex 4
    0.956862745,0.262745098,0.211764706,// vertex 3
    0,0,0,// vertex 7

    0,0,0,// vertex 7
    1,0.756862745,0.02745098,// vertex 6
    0,0,0,// vertex 4

    0,0,0,// vertex 7
    0.956862745,0.262745098,0.211764706,// vertex 3
    0,0,0,// vertex 2

    0,0,0,// vertex 2
    1,0.756862745,0.02745098,// vertex 8
    0,0,0,// vertex 7
  };
  tile_mesh = create3DObject(GL_TRIANGLES, 8*3, vertex_buffer_data, color_buffer_data, GL_LINE);
}

void createTileBase()
{
  GLfloat vertex_buffer_data [] = {
    1,-0.01f+0,1,
    1,-0.01f+0,-1,
    -1,-0.01f+0,-1,

    -1,-0.01f+0,-1,
    -1,-0.01f+0,1,
    1,-0.01f+0,1,

    1,-0.01f+th,1,
    1,-0.01f+th,-1,
    -1,-0.01f+th,-1,

    -1,-0.01f+th,-1,
    -1,-0.01f+th,1,
    1,-0.01f+th,1,

    1,-0.01f+0,1,
    1,-0.01f+0,-1,
    1,-0.01f+th,-1,

    1,-0.01f+th,-1,
    1,-0.01f+th,1,
    1,-0.01f+0,1,

    -1,-0.01f+0,1,
    -1,-0.01f+0,-1,
    -1,-0.01f+th,-1,

    -1,-0.01f+th,-1,
    -1,-0.01f+th,1,
    -1,-0.01f+0,1,

    -1,-0.01f+th,-1,
    1,-0.01f+th,-1,
    1,-0.01f+0,-1,

    1,-0.01f+0,-1,
    -1,-0.01f+0,-1,
    -1,-0.01f+th,-1,

    -1,-0.01f+th,1,
    1,-0.01f+th,1,
    1,-0.01f+0,1,

    1,-0.01f+0,1,
    -1,-0.01f+0,1,
    -1,-0.01f+th,1,
  };

  GLfloat color_buffer_data [] = {
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
    0,0,0,
  };
  tile_base = create3DObject(GL_TRIANGLES, 12*3, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void move3D(glm::vec3 initialTranslate, glm::vec3 translateVec,float rotationDegree, glm::vec3 rotateAboutVec, glm::vec3 scaleVec, VAO* OBJ, glm::mat4 MVP, glm::mat4 VP)
{
  Matrices.model = glm::mat4(1.0f);
  glm::mat4 initialTranslateOBJ = glm::translate(initialTranslate);
  glm::mat4 finalTranslateOBJ = glm::translate(glm::vec3(-initialTranslate.x, -initialTranslate.y, -initialTranslate.z));
  glm::mat4 translateOBJ = glm::translate (translateVec); // glTranslatef
  glm::mat4 rotateOBJ = glm::rotate((float)(rotationDegree*M_PI/180.0f), rotateAboutVec);  // rotate about vector (1,0,0)
  glm::mat4 scaleOBJ = glm::scale(scaleVec);
  glm::mat4 OBJTransform = translateOBJ * finalTranslateOBJ * rotateOBJ * initialTranslateOBJ * scaleOBJ;
  Matrices.model *= OBJTransform;
  MVP = VP * Matrices.model; // MVP = p * V * M
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
  draw3DObject(OBJ);
}

void draw (GLFWwindow* window)
{
  glm::mat4 VP;
  glm::mat4 MVP;	// MVP = Projection * View * Model
  glUseProgram(programID);
  glm::vec3 eye (2+CameraXOffset, 13+CameraYOffset, 6+CameraZOffset);
  glm::vec3 target (1+tx, 0+ty, 1+tz);
  glm::vec3 up (0+upx, 1+upy, 0+upz);

  // Compute Camera matrix (view)
  Matrices.view = glm::lookAt(eye, target, up); // Fixed camera for 2D (ortho) in XY plane
  // Matrices.projection = glm::perspective(glm::radians(120.0f), 16/9, 0.1f, 500.0f);
  if (proj_type == 1)
  {
    int fbwidth2, fbheight2;
    glfwGetFramebufferSize(window, &fbwidth2, &fbheight2);
    Matrices.projectionP = glm::perspective((GLfloat)(fov*M_PI/180), (GLfloat) fbwidth2 / (GLfloat) fbheight2, 0.1f, 100.0f);
  }


  // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
  VP = (proj_type?Matrices.projectionP:Matrices.projectionO) * Matrices.view;
  float scaleFactor = 1.0f;
  for (tileIndex = 0; tileIndex<170; tileIndex++)
  {
    if (Info[Level][tileIndex][0]!=5)
    {
      move3D(glm::vec3(0,0,0), glm::vec3(2*(tileIndex%17)-16, 0, 2*(tileIndex/17)-10), 0, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(scaleFactor, scaleFactor, scaleFactor), tile_base, MVP, VP);
      move3D(glm::vec3(0,0,0), glm::vec3(2*(tileIndex%17)-16, 0, 2*(tileIndex/17)-10), 0, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(scaleFactor, scaleFactor, scaleFactor), tile[Info[Level][tileIndex][0]], MVP, VP);
      move3D(glm::vec3(0,0,0), glm::vec3(2*(tileIndex%17)-16, 0, 2*(tileIndex/17)-10), 0, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(scaleFactor, scaleFactor, scaleFactor), tile_mesh, MVP, VP);
    }
  }
  if (ShiftFlag == 0)
  {
    move3D(glm::vec3(0,0,0), glm::vec3(-1+BoxOffsetX, 0+BoxOffsetY, -1+BoxOffsetZ), boxDegree, boxRotateAboutVec, glm::vec3(boxScaleX, boxScaleY, boxScaleZ), box, MVP, VP);
    move3D(glm::vec3(0,0,0), glm::vec3(-1+BoxOffsetX, 0+BoxOffsetY, -1+BoxOffsetZ), boxDegree, boxRotateAboutVec, glm::vec3(boxScaleX, boxScaleY, boxScaleZ), box_mesh, MVP, VP);
  }
  else if (ShiftFlag == 1) //movingright
  {
    move3D(glm::vec3(-2,0,0), glm::vec3(-1+BoxOffsetX, 0+BoxOffsetY, -1+BoxOffsetZ), boxDegree, boxRotateAboutVec, glm::vec3(boxScaleX, boxScaleY, boxScaleZ), box, MVP, VP);
    move3D(glm::vec3(-2,0,0), glm::vec3(-1+BoxOffsetX, 0+BoxOffsetY, -1+BoxOffsetZ), boxDegree, boxRotateAboutVec, glm::vec3(boxScaleX, boxScaleY, boxScaleZ), box_mesh, MVP, VP);
  }
  else if (ShiftFlag == 2)
  {
    move3D(glm::vec3(0,0,-2), glm::vec3(-1+BoxOffsetX, 0+BoxOffsetY, -1+BoxOffsetZ), boxDegree, boxRotateAboutVec, glm::vec3(boxScaleX, boxScaleY, boxScaleZ), box, MVP, VP);
    move3D(glm::vec3(0,0,-2), glm::vec3(-1+BoxOffsetX, 0+BoxOffsetY, -1+BoxOffsetZ), boxDegree, boxRotateAboutVec, glm::vec3(boxScaleX, boxScaleY, boxScaleZ), box_mesh, MVP, VP);
  }
  else if (ShiftFlag == 3) //movingright
  {
    move3D(glm::vec3(-4,0,0), glm::vec3(-1+BoxOffsetX, 0+BoxOffsetY, -1+BoxOffsetZ), boxDegree, boxRotateAboutVec, glm::vec3(boxScaleX, boxScaleY, boxScaleZ), box, MVP, VP);
    move3D(glm::vec3(-4,0,0), glm::vec3(-1+BoxOffsetX, 0+BoxOffsetY, -1+BoxOffsetZ), boxDegree, boxRotateAboutVec, glm::vec3(boxScaleX, boxScaleY, boxScaleZ), box_mesh, MVP, VP);
  }
  else if (ShiftFlag == 4)
  {
    move3D(glm::vec3(0,0,-4), glm::vec3(-1+BoxOffsetX, 0+BoxOffsetY, -1+BoxOffsetZ), boxDegree, boxRotateAboutVec, glm::vec3(boxScaleX, boxScaleY, boxScaleZ), box, MVP, VP);
    move3D(glm::vec3(0,0,-4), glm::vec3(-1+BoxOffsetX, 0+BoxOffsetY, -1+BoxOffsetZ), boxDegree, boxRotateAboutVec, glm::vec3(boxScaleX, boxScaleY, boxScaleZ), box_mesh, MVP, VP);
  }
}

GLFWwindow* initGLFW (int width, int height)
{
  GLFWwindow* window; // window desciptor/handle
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);
  if (!window) {
    exit(EXIT_FAILURE);
    glfwTerminate();
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval( 1 );
  // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetFramebufferSizeCallback(window, reshapeWindow);
  glfwSetWindowSizeCallback(window, reshapeWindow);
  glfwSetWindowCloseCallback(window, quit);
  glfwSetKeyCallback(window, keyboard);      // general keyboard input
  glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling
  glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks
  // glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  return window;
}

void initGL (GLFWwindow* window, int width, int height)
{
  createTile();
  createFragileTile();
  createSwitchTile();
  createEndTile();
  createTileMesh();
  createBox();
  createBoxMesh();
  createTileBase();
  programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
  Matrices.MatrixID = glGetUniformLocation(programID, "MVP");
  reshapeWindow (window, width, height);
  glClearColor (0.376470588f, 0.490196078f, 0.545098039f, 1.0f); // R, G, B, A
  glClearDepth (1.0f);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LEQUAL);
  // glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

double a, b, cur_x, cur_y, x_dif, y_dif;
int fbwidth3=1920, fbheight3=1080;
void cameraSetting(GLFWwindow* window){
    glfwGetCursorPos(window,&a,&b);
    glfwGetFramebufferSize(window, &fbwidth3, &fbheight3);
    if(MouseLeftInitial == 1)
    {
      cur_x=(a-fbwidth3)/16;
      cur_y=(b-fbheight3)/9;
      tx=-1; tz=-1;
      MouseLeftInitial=0;
    }
    if(MouseLeftClick == 1)
    {
        MouseLeftClickX=(a-fbwidth3)/16;
        MouseLeftClickY=(b-fbheight3)/9;
        x_dif = cur_x - MouseLeftClickX;
        y_dif = cur_y + MouseLeftClickY;
        CameraXOffset = -2+10*sin((4*x_dif)*M_PI/180.0f);
        CameraZOffset = -6+10*cos((4*x_dif)*M_PI/180.0f);
        CameraYOffset = 0;
    }
}

int main (int argc, char** argv)
{
  int width = 1920;
  int height = 1080;
  float framerate = 60.0f, camSpeed=4.0f;
  int animationSpeed = 6, BoxLife=1, messageToggle=0;
  GLFWwindow* window = initGLFW(width, height);
  initGLEW();
  initGL (window, width, height);

  createLevels();
  BoxOffsetX=BoxStart[Level][0]; BoxOffsetZ=BoxStart[Level][1];
  cameraSetting(window);

  // Compile and setup the shader
  Shader shader("fontrender.vert", "fontrender.frag");
  glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(fbwidth3), 0.0f, static_cast<GLfloat>(fbheight3));
  shader.Use();
  glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

  // FreeType
  FT_Library ft;
  // All functions return a value different than 0 whenever an error occurred
  if (FT_Init_FreeType(&ft))
      std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

  // Load font as face
  FT_Face face;
  if (FT_New_Face(ft, "arial.ttf", 0, &face))
      std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

  // Set size to load glyphs as
  FT_Set_Pixel_Sizes(face, 0, 48);

  // Disable byte-alignment restriction
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Load first 128 characters of ASCII set
  for (GLubyte c = 0; c < 128; c++)
  {
      // Load character glyph
      if (FT_Load_Char(face, c, FT_LOAD_RENDER))
      {
          std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
          continue;
      }
      // Generate texture
      GLuint texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(
          GL_TEXTURE_2D,
          0,
          GL_RED,
          face->glyph->bitmap.width,
          face->glyph->bitmap.rows,
          0,
          GL_RED,
          GL_UNSIGNED_BYTE,
          face->glyph->bitmap.buffer
      );
      // Set texture options
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // Now store character for later use
      Character character = {
          texture,
          glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
          glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
          static_cast<GLuint>(face->glyph->advance.x)
      };
      Characters.insert(std::pair<GLchar, Character>(c, character));
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  // Destroy FreeType once we're finished
  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  while (!glfwWindowShouldClose(window)) {
    cameraSetting(window);
    current_time = glfwGetTime();
    if ((current_time - last_update_time) >= (float)(1.0f/framerate)) {
      if (CamKeyA == 1)
        CameraXOffset+=camSpeed*(float)(-1.0f/framerate);
      if (CamKeyD == 1)
        CameraXOffset+=camSpeed*(float)(1.0f/framerate);
      if (CamKeyW == 1)
        CameraZOffset+=camSpeed*(float)(-1.0f/framerate);
      if (CamKeyS == 1)
        CameraZOffset+=camSpeed*(float)(1.0f/framerate);
      if (CamKeyZ == 1)
        CameraYOffset+=camSpeed*(float)(-1.0f/framerate);
      if (CamKeyX == 1)
        CameraYOffset+=camSpeed*(float)(1.0f/framerate);
      if (BoxLife==1)
      {
        if (BoxIsStanding == 1)
        {
          if (BoxFlagUp == 1)
          {
            boxRotateAboutVec = glm::vec3(-1,0,0);
            boxDegree = boxDegree + (float)(animationSpeed*90.0f/framerate);
            if (boxDegree == 90.0f)
            {
              BoxFlagUp = 0;
              boxDegree = 0; Moves++;
              boxRotateAboutVec = glm::vec3(0,0,1);
              BoxOffsetZ+=-4;
              boxScaleY=0.5f;
              boxScaleZ=2.0f;
              BoxIsStanding = 0;
              system("mpg123 -n 30 -i -q audio/anvil.mp3 &");
            }
          }
          if (BoxFlagDown == 1)
          {
            ShiftFlag = 2;
            boxRotateAboutVec = glm::vec3(1,0,0);
            boxDegree = boxDegree + (float)(animationSpeed*90.0f/framerate);
            if (boxDegree == 90.0f)
            {
              BoxFlagDown = 0;
              ShiftFlag = 0;
              boxDegree = 0; Moves++;
              boxRotateAboutVec = glm::vec3(0,0,1);
              BoxOffsetZ+=2;
              boxScaleY=0.5f;
              boxScaleZ=2.0f;
              BoxIsStanding = 0;
              system("mpg123 -n 30 -i -q audio/anvil.mp3 &");
            }
          }
          if (BoxFlagLeft == 1)
          {
            boxRotateAboutVec = glm::vec3(0,0,1);
            boxDegree = boxDegree + (float)(animationSpeed*90.0f/framerate);
            if (boxDegree == 90.0f)
            {
              BoxFlagLeft = 0;
              boxDegree = 0; Moves++;
              boxRotateAboutVec = glm::vec3(0,0,1);
              BoxOffsetX+=-4;
              boxScaleY=0.5f;
              boxScaleX=2.0f;
              BoxIsStanding = 0;
              system("mpg123 -n 30 -i -q audio/anvil.mp3 &");
            }
          }
          if (BoxFlagRight == 1)
          {
            ShiftFlag = 1;
            boxRotateAboutVec = glm::vec3(0,0,-1);
            boxDegree = boxDegree + (float)(animationSpeed*90.0f/framerate);
            if (boxDegree == 90.0f)
            {
              BoxFlagRight = 0;
              ShiftFlag = 0;
              boxDegree = 0; Moves++;
              boxRotateAboutVec = glm::vec3(0,0,1);
              BoxOffsetX+=2;
              boxScaleY=0.5f;
              boxScaleX=2.0f;
              BoxIsStanding = 0;
              system("mpg123 -n 30 -i -q audio/anvil.mp3 &");
            }
          }
        }
        else if (BoxIsStanding == 0)
        {
          if (boxScaleX == 2.0f)
          {
            if (BoxFlagUp == 1)
            {
              boxRotateAboutVec = glm::vec3(-1,0,0);
              boxDegree = boxDegree + (float)(animationSpeed*90.0f/framerate);
              if (boxDegree == 90.0f)
              {
                BoxFlagUp = 0;
                boxDegree = 0; Moves++;
                boxRotateAboutVec = glm::vec3(0,0,1);
                BoxOffsetZ+=-2;
                system("mpg123 -n 30 -i -q audio/anvil.mp3 &");
              }
            }
            if (BoxFlagDown == 1)
            {
              ShiftFlag = 2;
              boxRotateAboutVec = glm::vec3(1,0,0);
              boxDegree = boxDegree + (float)(animationSpeed*90.0f/framerate);
              if (boxDegree == 90.0f)
              {
                BoxFlagDown = 0;
                ShiftFlag = 0;
                boxDegree = 0; Moves++;
                boxRotateAboutVec = glm::vec3(0,0,1);
                BoxOffsetZ+=2;
                system("mpg123 -n 30 -i -q audio/anvil.mp3 &");
              }
            }
            if (BoxFlagLeft == 1)
            {
              boxRotateAboutVec = glm::vec3(0,0,1);
              boxDegree = boxDegree + (float)(animationSpeed*90.0f/framerate);
              if (boxDegree == 90.0f)
              {
                BoxFlagLeft = 0;
                boxDegree = 0; Moves++;
                boxRotateAboutVec = glm::vec3(0,0,1);
                BoxOffsetX+=-2;
                boxScaleY=1.0f;
                boxScaleX=1.0f;
                BoxIsStanding = 1;
                system("mpg123 -n 30 -i -q audio/anvil.mp3 &");
              }
            }
            if (BoxFlagRight == 1)
            {
              ShiftFlag = 3;
              boxRotateAboutVec = glm::vec3(0,0,-1);
              boxDegree = boxDegree + (float)(animationSpeed*90.0f/framerate);
              if (boxDegree == 90.0f)
              {
                BoxFlagRight = 0;
                ShiftFlag = 0;
                boxDegree = 0; Moves++;
                boxRotateAboutVec = glm::vec3(0,0,1);
                BoxOffsetX+=4;
                boxScaleY=1.0f;
                boxScaleX=1.0f;
                BoxIsStanding = 1;
                system("mpg123 -n 30 -i -q audio/anvil.mp3 &");
              }
            }
          }
          if (boxScaleZ == 2.0f)
          {
            if (BoxFlagUp == 1)
            {
              boxRotateAboutVec = glm::vec3(-1,0,0);
              boxDegree = boxDegree + (float)(animationSpeed*90.0f/framerate);
              if (boxDegree == 90.0f)
              {
                BoxFlagUp = 0;
                boxDegree = 0; Moves++;
                boxRotateAboutVec = glm::vec3(0,0,1);
                BoxOffsetZ+=-2;
                boxScaleY=1.0f;
                boxScaleZ=1.0f;
                BoxIsStanding=1;
                system("mpg123 -n 30 -i -q audio/anvil.mp3 &");
              }
            }
            if (BoxFlagDown == 1)
            {
              ShiftFlag = 4;
              boxRotateAboutVec = glm::vec3(1,0,0);
              boxDegree = boxDegree + (float)(animationSpeed*90.0f/framerate);
              if (boxDegree == 90.0f)
              {
                BoxFlagDown = 0;
                ShiftFlag = 0;
                boxDegree = 0; Moves++;
                boxRotateAboutVec = glm::vec3(0,0,1);
                BoxOffsetZ+=4;
                boxScaleY=1.0f;
                boxScaleZ=1.0f;
                BoxIsStanding=1;
                system("mpg123 -n 30 -i -q audio/anvil.mp3 &");
              }
            }
            if (BoxFlagLeft == 1)
            {
              boxRotateAboutVec = glm::vec3(0,0,1);
              boxDegree = boxDegree + (float)(animationSpeed*90.0f/framerate);
              if (boxDegree == 90.0f)
              {
                BoxFlagLeft = 0;
                boxDegree = 0; Moves++;
                boxRotateAboutVec = glm::vec3(0,0,1);
                BoxOffsetX+=-2;
                system("mpg123 -n 30 -i -q audio/anvil.mp3 &");
              }
            }
            if (BoxFlagRight == 1)
            {
              ShiftFlag = 1;
              boxRotateAboutVec = glm::vec3(0,0,-1);
              boxDegree = boxDegree + (float)(animationSpeed*90.0f/framerate);
              if (boxDegree == 90.0f)
              {
                BoxFlagRight = 0;
                ShiftFlag = 0;
                boxDegree = 0; Moves++;
                boxRotateAboutVec = glm::vec3(0,0,1);
                BoxOffsetX+=2;
                system("mpg123 -n 30 -i -q audio/anvil.mp3 &");
              }
            }
          }
        }
      }
      if ((-1+BoxOffsetX <= -19) || (-1+BoxOffsetX >= 16) || (-1+BoxOffsetZ <= -12) || (-1+BoxOffsetZ >= 8))
      {
        BoxFlagUp = 0; BoxFlagDown = 0; BoxFlagLeft = 0; BoxFlagRight = 0;
        system("mpg123 -n 10 -i -q audio/fail.mp3 &");
        GameEndingScreen = 1; glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);glBlendEquation(GL_FUNC_ADD);
        // glEnable(GL_BLEND);
// glBlendFunc(GL_ONE, GL_ZERO);
        if (messageToggle==1)
        {
          printf("You failed. The level is restarting.\n");
          printf("Level: %d Moves: %d\n", Level+1, Moves);
        }
        BoxOffsetY-=(float)(30.0f/framerate);
        BoxLife=0;
      }
      if (BoxOffsetY <= -30.0f && BoxLife == 0)
      {
        BoxLife=1;
        BoxOffsetX=BoxStart[Level][0];
        BoxOffsetY=0;
        BoxOffsetZ=BoxStart[Level][1];
        boxScaleX=1.0f;
        boxScaleY=1.0f;
        boxScaleZ=1.0f;
        BoxIsStanding=1;
        if (TileUnderBox == 4)
        {
          Level++;
          BoxOffsetX=BoxStart[Level][0];
          BoxOffsetZ=BoxStart[Level][1];
        }
      }
      // to know TileUnderBox
      if (BoxIsStanding == 1)
      {
        TileUnderBox = Info[Level][(int)(17*(5+BoxOffsetZ/2)+(8+BoxOffsetX/2))][0];
        tileIndex2 = (int)(17*(5+BoxOffsetZ/2)+(8+BoxOffsetX/2));
      }
      else if (BoxIsStanding == 0)
      {
        if (boxScaleZ == 2.0f)
        {
          if ((BoxOffsetZ >= -10.0f) && (BoxOffsetZ <= 6.0f))
          {
            TileUnderBox = Info[Level][(int)(17*(5+BoxOffsetZ/2)+(8+BoxOffsetX/2))][0];
            tileIndex2 = (int)(17*(5+BoxOffsetZ/2)+(8+BoxOffsetX/2));
            if (TileUnderBox == 1)
            {
              TileUnderBox = Info[Level][(int)(17*(6+BoxOffsetZ/2)+(8+BoxOffsetX/2))][0];
              tileIndex2 = (int)(17*(6+BoxOffsetZ/2)+(8+BoxOffsetX/2));
            }
          }
        }
        if (boxScaleX == 2.0f)
        {
          if ((BoxOffsetX >= -16.0f) && (BoxOffsetX <= 14.0f))
          {
            TileUnderBox = Info[Level][(int)(17*(5+BoxOffsetZ/2)+(8+BoxOffsetX/2))][0];
            tileIndex2 = (int)(17*(5+BoxOffsetZ/2)+(8+BoxOffsetX/2));
            if (TileUnderBox == 1)
            {
              TileUnderBox = Info[Level][(int)(17*(5+BoxOffsetZ/2)+(9+BoxOffsetX/2))][0];
              tileIndex2 = (int)(17*(5+BoxOffsetZ/2)+(9+BoxOffsetX/2));
            }
          }
        }
      }

      if (TileUnderBox == 5)
      {
        BoxFlagUp = 0; BoxFlagDown = 0; BoxFlagLeft = 0; BoxFlagRight = 0;
        system("mpg123 -n 10 -i -q audio/fail.mp3 &");
        GameEndingScreen = 1; glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        printf("You failed. The level is restarting.\n");
        printf("Level: %d Moves: %d\n", Level+1, Moves);
        BoxOffsetY-=(float)(30.0f/framerate);
        BoxLife=0;
      }
      if ((TileUnderBox == 4) && (BoxIsStanding == 1))
      {
        if (Level == Max_Level)
        {
          printf("You finished the game. Congrats.\n");
          printf("Level: %d Moves: %d\n", Level+1, Moves);
          GameEndingScreen = 2; glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
          system("mpg123 -n 10 -i -q audio/tada.mp3 &");
          printf("You passed a level. Keep it up!\n");
          printf("Level: %d Moves: %d\n", Level+1, Moves);
        }
        BoxOffsetY-=(float)(30.0f/framerate);
        BoxLife=0;
      }
      if ((TileUnderBox == 3) && ( MovesBackup != Moves))
      {
        MovesBackup = Moves;
        Info[Level][Info[Level][tileIndex2][1]][0] = 6-Info[Level][Info[Level][tileIndex2][1]][0];
        system("mpg123 -n 30 -i -q audio/horn.mp3 &");
      }
      if ((TileUnderBox == 2) && (BoxIsStanding == 1))
      {
        BoxFlagUp = 0; BoxFlagDown = 0; BoxFlagLeft = 0; BoxFlagRight = 0;
        system("mpg123 -n 30 -i -q audio/glass.mp3 &");
        printf("You broke a fragile tile. The level is restarting.\n");
        printf("Level: %d Moves: %d\n", Level+1, Moves);
        Info[Level][tileIndex2][0] = 2;
        BoxOffsetY-=(float)(30.0f/framerate);
        BoxLife=0;
      }
      if (BoxViewFlag == 1)
      {
        if (proj_type == 0)
        {
          CameraXOffset=0;
          CameraYOffset=0;
          CameraZOffset=0;
          upy=0;
          upz=0;
          tx=0;
          tz=0;
          proj_type=1;
        }
        CameraXOffset=-2+(int)(BoxOffsetX+2);
        CameraYOffset=-11+2;
        CameraZOffset=-8+(int)(BoxOffsetZ+2);
        tx=-1+(int)(2*(BoxStart[Level][2]%17)-16);
        ty=CameraYOffset;
        tz=-1+(int)(2*(BoxStart[Level][2]/17)-10);
      }
      if (FollowCam == 1)
      {
        if (proj_type == 0)
        {
          CameraXOffset=0;
          CameraYOffset=0;
          CameraZOffset=0;
          upy=0;
          upz=0;
          tx=0;
          tz=0;
          proj_type=1;
        }
        CameraXOffset=-2+(int)(BoxOffsetX-1);
        CameraYOffset=-13+8;
        CameraZOffset=-6+(int)(BoxOffsetZ-1);
        tx=-1+(int)(BoxOffsetX);
        ty=CameraYOffset;
        tz=-1+(int)(BoxOffsetZ);
      }
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      if ((GameStartingScreen == 0) && (GameEndingScreen == 0))
      {
        draw(window);

        // draw the scoreboard
        glGenVertexArrays(1, &fontVAO);
        glGenBuffers(1, &fontVBO);
        glBindVertexArray(fontVAO);
        glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_BLEND);
        Level2=Level+1;
        message = "Level: " + std::to_string(Level2) + " Moves: " + std::to_string(Moves);
        RenderText(shader, message, float(fbwidth3)*(0.80), float(fbheight3)*(0.9), 0.6f, glm::vec3(0.54296875, 0.76171875, 0.2890625));
      }

      if (GameStartingScreen == 1)
      {
        glGenVertexArrays(1, &fontVAO);
        glGenBuffers(1, &fontVBO);
        glBindVertexArray(fontVAO);
        glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        RenderText(shader, "Bloxorz (Almost)", float(fbwidth3/2)*(0.75), float(fbheight3/2)*(1.3), 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(shader, "Press ENTER to start the game.", float(fbwidth3/2)*(0.65), float(fbheight3/2)*(1.1), 0.8f, glm::vec3(0.3, 0.7f, 0.9f));
        RenderText(shader, "Press Q to quit the game.", float(fbwidth3/2)*(0.70), float(fbheight3/2)*(0.9), 0.8f, glm::vec3(0.3, 0.7f, 0.9f));
        RenderText(shader, "Done by Tarun Gavara (201501147) as part of Graphics Assignment-2.", float(fbwidth3/2)*(0.55), float(fbheight3/2)*(0.2), 0.5f, glm::vec3(0.5, 0.8f, 0.2f));
        // printf("%d %d\n", fbwidth3, fbheight3);
      }
      if (GameEndingScreen == 1)
      {
        glGenVertexArrays(1, &fontVAO);
        glGenBuffers(1, &fontVBO);
        glBindVertexArray(fontVAO);
        glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
        RenderText(shader, "You failed. Game Over.", float(fbwidth3/2)*(0.70), float(fbheight3/2)*(1.3), 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(shader, "Press ENTER to restart the game.", float(fbwidth3/2)*(0.65), float(fbheight3/2)*(1.1), 0.8f, glm::vec3(0.3, 0.7f, 0.9f));
        RenderText(shader, "Press Q to quit the game.", float(fbwidth3/2)*(0.70), float(fbheight3/2)*(0.9), 0.8f, glm::vec3(0.3, 0.7f, 0.9f));
        Level2=Level+1;
        message = "Level: " + std::to_string(Level2) + " Moves: " + std::to_string(Moves);
        RenderText(shader, message, float(fbwidth3/2)*(0.80), float(fbheight3/2)*(0.7), 0.8f, glm::vec3(0.3, 0.7f, 0.9f));
      }
      if (GameEndingScreen == 2)
      {
        glGenVertexArrays(1, &fontVAO);
        glGenBuffers(1, &fontVBO);
        glBindVertexArray(fontVAO);
        glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
        RenderText(shader, "You finished all levels. Congrats.", float(fbwidth3/2)*(0.50), float(fbheight3/2)*(1.3), 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText(shader, "Press ENTER to restart the game.", float(fbwidth3/2)*(0.65), float(fbheight3/2)*(1.1), 0.8f, glm::vec3(0.3, 0.7f, 0.9f));
        RenderText(shader, "Press Q to quit the game.", float(fbwidth3/2)*(0.70), float(fbheight3/2)*(0.9), 0.8f, glm::vec3(0.3, 0.7f, 0.9f));
        Level2=Level+1;
        message = "Level: " + std::to_string(Level2) + " Moves: " + std::to_string(Moves);
        RenderText(shader, message, float(fbwidth3/2)*(0.80), float(fbheight3/2)*(0.7), 0.8f, glm::vec3(0.3, 0.7f, 0.9f));
      }

      glfwSwapBuffers(window);
      glfwPollEvents();
      last_update_time = current_time;
    }
    if ((current_time - last_update_time2) >= 1.0) { // atleast 0.5s elapsed since last frame
      // messageToggle=1-messageToggle;
      // if (GameEndingScreen == 1)
      //   glEnable(GL_BLEND);
      last_update_time2 = current_time;
    }
  }
  glfwTerminate();
}

void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state
    shader.Use();
    glUniform3f(glGetUniformLocation(shader.Program, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(fontVAO);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
