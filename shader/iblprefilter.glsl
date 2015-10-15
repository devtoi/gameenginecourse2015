#new_shader compute
#version 430
layout(local_size_x = 16, local_size_y = 16) in;

#define saturate(x) clamp(x,0.0f,1.0f)
#define PI 3.141592653589793

layout(binding = 0, rg16) restrict uniform image2D Tex;

float radicalInverse_VdC(uint bits) {
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
 }
 // http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
 vec2 Hammersley(uint i, uint N) {
     return vec2(float(i)/float(N), radicalInverse_VdC(i));
 }

vec3 ImportanceSampleGGX(vec2 Xi, float Roughness, vec3 N)
{
	float a = Roughness * Roughness;

	// Compute distribution direction
	float Phi = 2 * PI * Xi.x;
	float CosTheta = sqrt((1 - Xi.y) / (1 + (a*a - 1) * Xi.y));		
	float SinTheta = sqrt(1 - CosTheta * CosTheta);

	// Convert to spherical direction
	vec3 H;
	H.x = SinTheta * cos(Phi);
	H.y = SinTheta * sin(Phi);
	H.z = CosTheta;

	vec3 UpVector = abs(N.z) < 0.999 ? vec3(0, 0, 1) : vec3(1, 0, 0);
	vec3 TangentX = normalize(cross(UpVector, N));
	vec3 TangentY = cross(N, TangentX);

	// Tangent to world space
	return TangentX * H.x + TangentY * H.y + N * H.z;
}
float G1(
	float ndw, // w is either Ln or Vn
	float k)
{
// One generic factor of the geometry function divided by ndw
// NB : We should have k > 0
	return 1.0 / ( ndw*(1.0-k) + k );
}
float visibility(
	float ndl,
	float ndv,
	float Roughness)
{
// Schlick with Smith-like choice of k
// cf http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf p3
// visibility is a Cook-Torrance geometry function divided by (n.l)*(n.v)
	float k = Roughness * Roughness * 0.5;
	return G1(ndl,k)*G1(ndv,k);
}

// http://graphicrants.blogspot.com.au/2013/08/specular-brdf-reference.html
float GGX(float NdotV, float a)
{
	float k = a / 2;
	return NdotV / (NdotV * (1.0f - k) + k);
}

// http://graphicrants.blogspot.com.au/2013/08/specular-brdf-reference.html
float G_Smith(float a, float nDotV, float nDotL)
{
	return GGX(nDotL, a * a) * GGX(nDotV, a * a);
}
vec2 IntegrateBRDF(float roughness, float NoV){
	vec3 V; 
	V.x = sqrt(1.0 - NoV * NoV);
	V.y = 0;
	V.z = NoV;

	float A = 0;
	float B = 0;

	const uint numSamples = 1024;
	for(uint i = 0; i < numSamples; i++){
		vec2 Xi = Hammersley(i, numSamples);
		vec3 H = ImportanceSampleGGX(Xi, roughness, vec3(0,0,1));
		vec3 L = 2 * dot(V,H) * H - V;

		float NoL = saturate(L.z);
		float NoH = saturate(H.z);
		float VoH = saturate(dot(V,H));

		if(NoL > 0){
			float G = G_Smith(roughness, NoV, NoL );

			float G_vis =G * VoH / (NoH, NoV);
			float Fc = pow(1 - VoH, 5);
			A += (1 - Fc) * G_vis;
			B += Fc * G_vis;
		}

	}
	return vec2(A,B) / numSamples;
}
void main(){
	ivec2 threadIndex = ivec2( gl_GlobalInvocationID.xy );
	ivec2 texSize = imageSize(Tex);
	vec2 uv = vec2(threadIndex) / vec2(texSize);

	//precalc ibl
	vec4 color = vec4(IntegrateBRDF(uv.x ,uv.y),0,0);

	imageStore( Tex, threadIndex, color);
}
#end_shader