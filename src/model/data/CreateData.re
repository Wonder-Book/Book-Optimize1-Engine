open DataType;

let createState = () => {
  deviceManagerData: DeviceManager.createData(),
  glslData: Shader.GLSL.createGLSLData(),
  glslLocationData: Shader.GLSLLocation.createGLSLLocationData(),
  programData: Shader.Program.createProgramData(),
  cameraData: Camera.createCameraData(),
  viewData: View.createViewData(),
  allGameObjectData: GameObject.createAllGameObjectData(),
};