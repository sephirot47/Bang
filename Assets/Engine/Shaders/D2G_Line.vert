B_DRAW_TO_GBUFFER_VS_DECLARE();

uniform mat4 B_matrix_pvm;

in vec3 B_position_raw_vin;

out vec4 B_positionRaw_vout_fin;

void main()
{
    B_DRAW_TO_GBUFFER_VS_INIT_MAIN();

    B_positionRaw_vout_fin = vec4(B_position_raw_vin, 1.0);
    gl_Position = B_matrix_pvm * vec4(B_position_raw_vin, 1.0);
}
