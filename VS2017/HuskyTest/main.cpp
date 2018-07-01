#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Husky/Math/Matrix44.hpp>

int main()
{
  if (!glfwInit()) {
    return -1;
  }

  GLFWwindow *window = glfwCreateWindow(1280, 720, "Hello Husky!", NULL, NULL);
  if (window == nullptr) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO: Render something!

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
