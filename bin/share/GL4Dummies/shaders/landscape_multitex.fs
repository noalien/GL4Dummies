#version 330

#define M_PI 3.1415926535897932384626433832795

uniform sampler2D myTexture0;
uniform sampler2D myTexture1;
uniform sampler2D myTexture2;
uniform sampler2D myTexture3;
uniform sampler2D myTexture4;


uniform sampler2D myDetail;

uniform int hm;
uniform int type;
uniform float min_h;
uniform float max_h;

in vec2 vsoTexCoord;
in vec3 vsoNormal;

in mat3 TBN;
in vec3 tangentLightPos;
in vec3 tangentViewPos;
in vec3 tangentFragPos;

in float height;

out vec4 fragColor;


void main(void) {

  vec3 colorNormal=texture(myDetail,vsoTexCoord).rgb;
  colorNormal=normalize(colorNormal*2.0-1.0);
  colorNormal=vec3(1.0,1.0,1.0);
  vec3 normal=normalize(vsoNormal);

  vec3 color0=texture(myTexture0,vsoTexCoord).rgb;
  vec3 color1=texture(myTexture1,vsoTexCoord).rgb;
  vec3 color2=texture(myTexture2,vsoTexCoord).rgb;
  vec3 color3=texture(myTexture3,vsoTexCoord).rgb;
  vec3 color4=texture(myTexture4,vsoTexCoord).rgb;
  // vec3 color=vec3(1.0,1.0,1.0);

  float angle=asin(vsoNormal.y/sqrt(vsoNormal.x*vsoNormal.x+vsoNormal.y*vsoNormal.y+vsoNormal.z*vsoNormal.z));

  //eclairages basiques
  vec3 lDir=normalize(tangentLightPos-tangentFragPos);
  vec3 vDir=normalize(vec3(0,0,0)-tangentFragPos);
  vec3 reflectDir=reflect(-lDir, normal/*colorNormal*/);  
  vec3 halfwayDir=normalize(lDir+vDir);
  
  // vec3 ambient=0.1* color0;
  float diff=max(dot(normal/*colorNormal*/,lDir),0.0);
  // vec3 diffuse=diff*color0;
  float spec=pow(max(dot(normal/*colorNormal*/,halfwayDir),0.0),32.0);
  vec3 specular=vec3(0.3,0.3,0.3)*spec;  

  if(hm==1){

  	//PENTE
  	if(type==0||type==1){


 		if(abs(angle)/(M_PI/2)<0.20)
  			fragColor= vec4(0.1*color2+diff*color2+specular,1.0);
  		//DEGRADE
  		else if(abs(angle)/(M_PI/2)<0.35)
  			fragColor= vec4(0.1*color2+diff*color2+specular,1.0) * (1-( abs(angle)/(M_PI/2) - 0.20) * 1/(0.35-0.20)) + vec4(0.1*color0+diff*color0+specular,1.0) * ( abs(angle)/(M_PI/2) - 0.20) * 1/(0.35-0.20);   
  		else if(abs(angle)/(M_PI/2)<0.50)
  			fragColor= vec4(0.1*color0+diff*color0+specular,1.0);
  		//DEGRADE
  		else if(abs(angle)/(M_PI/2)<0.65)
  			fragColor= vec4(0.1*color0+diff*color0+specular,1.0) * (1-( abs(angle)/(M_PI/2) - 0.50) * 1/(0.65-0.50)) + vec4(0.1*color1+diff*color1+specular,1.0) * ( abs(angle)/(M_PI/2) - 0.50) * 1/(0.65-0.50);   
    	else if(abs(angle)/(M_PI/2)<1.1)
  			fragColor= vec4(0.1*color1+diff*color1+specular,1.0);

  		else
		    fragColor=vec4(1.0,1.0,1.0,1.0);
  	}
  	//HAUTEUR
  	else{

  		if(height>max_h+(max_h-min_h)*0.15)
  			fragColor= vec4(0.1*color4+diff*color4+specular,1.0);
  		//DEGRADE
  		else if(height>min_h+(max_h-min_h)*0.95)
  			fragColor= vec4(0.1*color4+diff*color4+specular,1.0) *  (height-(min_h+(max_h-min_h)*0.95)) / ((max_h+(max_h-min_h)*0.15)-(min_h+(max_h-min_h)*0.95)) + vec4(0.1*color2+diff*color2+specular,1.0) *  (1-(height-(min_h+(max_h-min_h)*0.95)) / ((max_h+(max_h-min_h)*0.15)-(min_h+(max_h-min_h)*0.9)));   
  		
  		else if(height>min_h+(max_h-min_h)*0.7)
  			fragColor= vec4(0.1*color2+diff*color2+specular,1.0);
  		//DEGRADE
  		else if(height>min_h+(max_h-min_h)*0.45)
  			fragColor= vec4(0.1*color2+diff*color2+specular,1.0) *  (height-(min_h+(max_h-min_h)*0.45)) / ((min_h+(max_h-min_h)*0.7)-(min_h+(max_h-min_h)*0.45)) + vec4(0.1*color1+diff*color1+specular,1.0) *  (1-(height-(min_h+(max_h-min_h)*0.45)) / ((min_h+(max_h-min_h)*0.7)-(min_h+(max_h-min_h)*0.45)));   

  		else if(height>min_h+(max_h-min_h)*0.25)
  			fragColor= vec4(0.1*color1+diff*color1+specular,1.0);
  		//DEGRADE
  		else if(height>min_h)
  			fragColor= vec4(0.1*color1+diff*color1+specular,1.0) *  (height-min_h) / ((min_h+(max_h-min_h)*0.25)-min_h) + vec4(0.1*color3+diff*color3+specular,1.0) *  (1-(height-min_h) / ((min_h+(max_h-min_h)*0.25)-min_h));   
  		
  		else
  			fragColor= vec4(0.1*color3+diff*color3+specular,1.0);



  	}
  }
  else
    fragColor=vec4(1.0,1.0,1.0,1.0);
}
  