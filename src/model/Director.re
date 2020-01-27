let initAll = (contextParam, state) => {
  let state =
    state
    |> DeviceManager.setGl(
         Gl.getWebgl1Context(View.unsafeGetCanvasByThrow(state), contextParam),
       );

  state |> GPUDetect.detect(DeviceManager.unsafeGetGlByThrow(state)) |> Shader.init;
};

let loopBody = state => {
  let gl = DeviceManager.unsafeGetGlByThrow(state);

  DeviceManager.clearColor(gl, state);

  Gl.clear(Gl.getColorBufferBit(gl) lor Gl.getDepthBufferBit(gl), gl);

  Render.render(gl, state);
};