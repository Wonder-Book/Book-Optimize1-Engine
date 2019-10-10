type deviceManagerData = {
  gl: option(GlType.webgl1Context),
  clearColor: (float, float, float, float),
};

type shaderName = string;

type vs = string;

type fs = string;

type fieldName = string;

type fieldNameArr = array(fieldName);

type glslData = {
  glslMap:
    ImmutableHashMap.t2(shaderName, ((vs, fs), fieldNameArr, fieldNameArr)),
};

type programData = {
  programMap: ImmutableHashMap.t2(shaderName, GlType.program),
};

type geometryData = {
  vertices: Js.Typed_array.Float32Array.t,
  indices: Js.Typed_array.Uint16Array.t,
  vertexBuffer: option(GlType.buffer),
  indexBuffer: option(GlType.buffer),
};

type transformData = {mMatrix: MatrixType.matrix};

type materialData = {
  shaderName,
  color: (float, float, float),
};

type gameObjectData = {
  transformData,
  geometryData,
  materialData,
};

type allGameObjectData = {gameObjectDataArr: array(gameObjectData)};

type cameraData = {
  vMatrix: option(MatrixType.matrix),
  pMatrix: option(MatrixType.matrix),
};

type canvas = DomExtendType.htmlElement;

type viewData = {canvas: option(canvas)};

type attributeLocationMap =
  ImmutableHashMap.t2(
    shaderName,
    ImmutableHashMap.t2(fieldName, GlType.attributeLocation),
  );

type uniformLocationMap =
  ImmutableHashMap.t2(
    shaderName,
    ImmutableHashMap.t2(fieldName, GlType.uniformLocation),
  );

type glslLocationData = {
  attributeLocationMap,
  uniformLocationMap,
};

type state = {
  viewData,
  deviceManagerData,
  glslData,
  glslLocationData,
  programData,
  cameraData,
  allGameObjectData,
};

type stateData = {
  mutable state: option(state),
  mutable isDebug: bool,
};