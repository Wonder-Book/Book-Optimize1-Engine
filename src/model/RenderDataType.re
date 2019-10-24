type renderData = {
  mMatrix: Js.Typed_array.Float32Array.t,
  vao: Gl.vao,
  indexCount: int,
  colors: list((float, float, float)),
  program: Gl.program,
  shaderName: string,
};