let initAll = (contextParam, state) => {
  let state =
    state
    |> DeviceManager.setGl(
         Gl.getWebgl1Context(View.unsafeGetCanvas(state), contextParam),
       );

  state |> GPUDetect.detect(DeviceManager.unsafeGetGl(state)) |> Shader.init;
};

let loopBody = state => {
  let gl = DeviceManager.unsafeGetGl(state);

  DeviceManager.clearColor(gl, state);

  Gl.clear(Gl.getColorBufferBit(gl) lor Gl.getDepthBufferBit(gl), gl);

  DeviceManager.initGlState(gl);

  Render.render(gl, state);
};