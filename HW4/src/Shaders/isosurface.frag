#version 460 compatibility
in vec3 fragPos;
in vec3 normal;
in float toClip;

out vec4 FragColor;

uniform vec3 eyePos;

uniform vec3 LightPosition0;
uniform vec3 LightDirection0;
uniform vec3 LightIntensity0;
uniform float LightCutoff0;

uniform bool makeCrossSection;

void main()
{
    if(!makeCrossSection && toClip < 0.0f)
        discard;

    if(makeCrossSection && abs(toClip) > 0.5)
        discard;
    
    vec3 ambientReflection = vec3(0.2f, 0.2f, 0.2f);
    vec3 diffuseReflection = vec3(0.6f, 0.3f, 0.1f);
    vec3 specularReflection = vec3(0.3f, 0.3f, 0.3f);

    vec3 ambient = ambientReflection * LightIntensity0;

    vec3 lightDir = normalize(LightPosition0 - fragPos);

    vec3 normalTemp = normal;
    if(dot(normal, lightDir) < 0)
    {
        normalTemp = -normal;
    }

    float diff = max(dot(normalTemp, lightDir), 0.0);
    vec3 diffuse = diff * LightIntensity0;

    vec3 viewDir = normalize(eyePos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normalTemp);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = specularReflection * spec * LightIntensity0;

    FragColor = vec4(clamp((ambient + diffuse + specular) * diffuseReflection, vec3(0.0f), vec3(1.0f)), 1.0f );
}