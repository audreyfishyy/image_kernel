#version 330 core

in vec2 texture_coordinate;

uniform sampler2D u_texture;
uniform vec2 u_texture_size;

// collumn major
mat3 get_surrounding_pixels(vec2 coordinate) 
{
	vec2 pixel_offset = 1.0 / u_texture_size;
	
	mat3 m;
	
	m[0][0] = texture(u_texture, coordinate + vec2(-1.0, 1.0) * pixel_offset).r;
	m[1][0] = texture(u_texture, coordinate + vec2(0.0, 1.0) * pixel_offset).r;
	m[2][0] = texture(u_texture, coordinate + vec2(1.0, 1.0) * pixel_offset).r;
	
	m[0][1] = texture(u_texture, coordinate + vec2(-1.0, 0.0) * pixel_offset).r;
	m[1][1] = texture(u_texture, coordinate + vec2(0.0, 0.0) * pixel_offset).r;
	m[2][1] = texture(u_texture, coordinate + vec2(1.0, 0.0) * pixel_offset).r;

	m[0][2] = texture(u_texture, coordinate + vec2(-1.0, -1.0) * pixel_offset).r;
	m[1][2] = texture(u_texture, coordinate + vec2(0.0, -1.0) * pixel_offset).r;
	m[2][2] = texture(u_texture, coordinate + vec2(1.0, -1.0) * pixel_offset).r;

	return m;
}

void main()
{			
	// edge
	/*
	-1.0, -1.0, -1.0,
	-1.0,  8.0, -1.0,
	-1.0, -1.0, -1.0
	*/
	// sharpen
	/*
	0.0, -1.0, 0.0,
	-1.0, 5.0, -1.0,
	0.0, -1.0, 0.0
	*/
	
	mat3 kernel = mat3(
		-1.0, -1.0, -1.0,
		-1.0,  8.0, -1.0,
		-1.0, -1.0, -1.0
	);
		
	mat3 pixel_matrix = get_surrounding_pixels(texture_coordinate);
	
	float result = 0.0;
	for (int i = 0; i < 3; i++) 
	{
		for (int j = 0; j < 3; j++) 
		{
			result += kernel[i][j] * pixel_matrix[i][j];
		}
	}

	result = clamp(result, 0.0, 1.0);
	gl_FragColor = vec4(result, result, result, 1.0);
}