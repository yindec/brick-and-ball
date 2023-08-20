#pragma once
#include <glad/glad.h>; // ����glad����ȡ���еı���OpenGLͷ�ļ�

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
	Shader(const char* vertexPath, const char* fragmentPath);
	// use/active program
	void use();
	// uniform tool functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
private:
	void checkCompileErrors(unsigned int shader, std::string type);
};