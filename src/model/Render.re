open DataType;

open RenderType;

let _createVBOs = ((vertices, indices), gl) => {
  let vertexBuffer = Gl.createBuffer(gl);

  Gl.bindBuffer(Gl.getArrayBuffer(gl), vertexBuffer, gl);

  Gl.bufferFloat32Data(
    Gl.getArrayBuffer(gl),
    vertices,
    Gl.getStaticDraw(gl),
    gl,
  );

  let indexBuffer = Gl.createBuffer(gl);

  Gl.bindBuffer(Gl.getElementArrayBuffer(gl), indexBuffer, gl);

  Gl.bufferUint16Data(
    Gl.getElementArrayBuffer(gl),
    indices,
    Gl.getStaticDraw(gl),
    gl,
  );

  (vertexBuffer, indexBuffer);
};

let _getOrCreateVBOs = ({vertices, indices, vertexBuffer, indexBuffer}, gl) =>
  switch (vertexBuffer, indexBuffer) {
  | (None, None) => _createVBOs((vertices, indices), gl)
  | _ => (vertexBuffer |> Option.unsafeGet, indexBuffer |> Option.unsafeGet)
  };

let _initVBOs = (gl, state) =>
  GameObject.getGameObjectDataArr(state)
  |> Js.Array.map(
       ({transformData, geometryData, materialData} as gameObjectData) =>
       {
         ...gameObjectData,
         geometryData:
           _getOrCreateVBOs(geometryData, gl)
           |> GameObject.Geometry.setBufferts(_, geometryData),
       }
     )
  |> GameObject.setGameObjectDataArr(_, state);

let _getProgram = ({shaderName}: materialData, state) =>
  Shader.Program.unsafeGetProgram(shaderName, state);

let _changeGameObjectDataArrToRenderDataArr = (gameObjectDataArr, gl, state) =>
  gameObjectDataArr
  |> Js.Array.map(
       ({transformData, geometryData, materialData} as gameObjectData) => {
       let (vertexBuffer, indexBuffer) =
         GameObject.Geometry.unsafeGetBuffers(geometryData);

       {
         mMatrix: GameObject.Transform.getMMatrix(transformData),
         vertexBuffer,
         indexBuffer,
         indexCount:
           GameObject.Geometry.getIndices(geometryData)
           |> Js.Typed_array.Uint16Array.length,
         color: GameObject.Material.getColor(materialData),
         program: _getProgram(materialData, state),
         shaderName: GameObject.Material.getShaderName(materialData),
       };
     });

let _sendAttributeData = (vertexBuffer, program, shaderName, gl, state) => {
  let positionLocation =
    Shader.GLSLLocation.unsafeGetAttribLocation(
      shaderName,
      "a_position",
      state,
    );

  Gl.bindBuffer(Gl.getArrayBuffer(gl), vertexBuffer, gl);

  Gl.vertexAttribPointer(
    positionLocation,
    3,
    Gl.getFloat(gl),
    false,
    0,
    0,
    gl,
  );
  Gl.enableVertexAttribArray(positionLocation, gl);
};

let _sendCameraUniformData =
    ((vMatrix, pMatrix), program, shaderName, gl, state) => {
  let vMatrixLocation =
    Shader.GLSLLocation.unsafeGetUniformLocation(
      shaderName,
      "u_vMatrix",
      state,
    );
  let pMatrixLocation =
    Shader.GLSLLocation.unsafeGetUniformLocation(
      shaderName,
      "u_pMatrix",
      state,
    );

  Gl.uniformMatrix4fv(vMatrixLocation, false, vMatrix, gl);
  Gl.uniformMatrix4fv(pMatrixLocation, false, pMatrix, gl);
};

let _sendModelUniformData =
    ((mMatrix, color), program, shaderName, gl, state) => {
  let mMatrixLocation =
    Shader.GLSLLocation.unsafeGetUniformLocation(
      shaderName,
      "u_mMatrix",
      state,
    );
  let colorFieldName = "u_color";
  let colorLocation =
    Shader.GLSLLocation.unsafeGetUniformLocation(
      shaderName,
      colorFieldName,
      state,
    );

  let (r, g, b) = color;

  Gl.uniformMatrix4fv(mMatrixLocation, false, mMatrix, gl);

  Shader.GLSLSender.setShaderCacheMap(
    shaderName,
    Shader.GLSLSender.unsafeGetShaderCacheMap(shaderName, state)
    |> Shader.GLSLSender.sendFloat3(
         gl,
         (colorFieldName, colorLocation),
         [|r, g, b|],
       ),
    state,
  );
};

let _sendUniformShaderData = (gl, state) => {
  let (vMatrix, pMatrix) = (
    Camera.unsafeGetVMatrix(state),
    Camera.unsafeGetPMatrix(state),
  );

  Shader.GLSL.getAllValidGLSLEntries(state)
  |> ArrayUtils.reduceOneParam(
       (. state, (shaderName, _)) => {
         let program = Shader.Program.unsafeGetProgram(shaderName, state);

         let state = Shader.Program.use(gl, program, state);

         _sendCameraUniformData(
           (vMatrix, pMatrix),
           program,
           shaderName,
           gl,
           state,
         );

         state
       },
       state,
     );
};

let render = (gl, state) => {
  let state = _initVBOs(gl, state);

  let state = _sendUniformShaderData(gl, state);

  _changeGameObjectDataArrToRenderDataArr(
    GameObject.getGameObjectDataArr(state),
    gl,
    state,
  )
  |> ArrayUtils.reduceOneParam(
       (.
         state,
         {
           mMatrix,
           vertexBuffer,
           indexBuffer,
           indexCount,
           color,
           program,
           shaderName,
         },
       ) => {
         let state = Shader.Program.use(gl, program, state);

         _sendAttributeData(vertexBuffer, program, shaderName, gl, state);

         let state =
           state
           |> _sendModelUniformData(
                (mMatrix, color),
                program,
                shaderName,
                gl,
              );

         Gl.bindBuffer(Gl.getElementArrayBuffer(gl), indexBuffer, gl);

         Gl.drawElements(
           Gl.getTriangles(gl),
           indexCount,
           Gl.getUnsignedShort(gl),
           0,
           gl,
         );

         state;
       },
       state,
     );
};