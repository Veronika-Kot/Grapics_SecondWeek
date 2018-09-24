#version 420   //GL version 4.2, shader language

//uniform mat4 MVP; // 4 by 4 matrix
uniform mat4 matModel; //M
uniform mat4 matView;  //V
uniform mat4 matProj;  //P

in vec3 vCol; // used to be attribute rgb
in vec3 vPos; //xyz vec2 for xy only
//if atributes are not enabled it will get to origin location 000 or black color

out vec3 color;  //out = going out to fragment Shader
					//varying (used to be) = could bedifferent for eacj\h vertex
					//uniform = ALL vertex shaders have the same value 

out vec4 vertPosWorld;
					
void main()
{
	vec3 posTemp = vPos;
	posTemp = posTemp * 4.0f;

	mat4 MVP = matProj * matView * matModel;

    gl_Position = MVP * vec4(posTemp, 1.0); // position in the screen
	vertPosWorld = matModel * vec4(posTemp, 1.0); //position in the world

    color = vCol; //take color which comes in and assigned it to the color, which goes to next change

}
