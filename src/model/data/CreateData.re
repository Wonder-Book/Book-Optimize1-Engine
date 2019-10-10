open DataType;

let createState = () => {
  deviceManagerData: DeviceManager.createData(),
  glslData: Shader.GLSL.createGLSLData(),
  programData: Shader.Program.createProgramData(),
  cameraData: Camera.createCameraData(),
  viewData: View.createViewData(),
  allGameObjectData: GameObject.createAllGameObjectData(),
};