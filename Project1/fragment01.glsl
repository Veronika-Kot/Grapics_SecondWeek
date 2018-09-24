#version 420

in vec3 color; //coming in from vertex shader
in vec4 vertPosWorld;

uniform vec3 objectColor;
uniform bool useVertexColour;

//Simple lighting; if object is located farther it will be darker
uniform vec3 lightPos;
uniform float lightBrightness;

out vec4 finalOutputColor; //going out to the screen; must be vec4

void main()
{

 if( useVertexColour )
 {
   finalOutputColor = vec4(color, 1.0);  //gl_FragColor = vec4(color, 1.0);
 }
 else {
	finalOutputColor = vec4(objectColor, 1.0);  //gl_FragColor = vec4(objectColor, 1.0);
 }

  //Calculating the distance between the "light"
  //and THIS vertex (in "world space")

  float distToLight = distance(vertPosWorld.xyz, lightPos.xyz);
  distToLight = abs(distToLight);

  float atten = (1.0f/distToLight) * lightBrightness;
  //Clamp out to 1.0f
  atten = min(1.0f, atten);

  finalOutputColor *= atten;
	
	//We can change color here
	//gl_FragColor.r = 1.0f;
	//gl_FragColor.gb = 0.0f;

}