#pragma once
#include <glad/glad.h>; // 包含glad来获取所有的必须OpenGL头文件
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	// Program ID
	unsigned int ID;

	// constructor and build shader
	Shader();
	Shader(const char* vertexPath, const char* fragmentPath);
	// use/active program
	void use();
	// uniform tool functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMatrix4(const std::string& name, const glm::mat4& matrix) const;
	void setVector2f(const std::string& name, const glm::vec2& value) const;
	void setVector3f(const std::string& name, const glm::vec3& value) const;
	void setVector4f(const std::string& name, const glm::vec4& value) const;
private:
	void checkCompileErrors(unsigned int shader, std::string type);
};