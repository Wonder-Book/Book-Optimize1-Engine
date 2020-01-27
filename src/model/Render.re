open DataType;

open RenderDataType;

let _createVAOs =
    (
      (vertices, indices),
      shaderName,
      gl,
      vaoExt: GPUDetectType.vaoExt,
      state,
    ) => {
  let vao = vaoExt##createVertexArrayOES();

  vaoExt##bindVertexArrayOES(. Js.Nullable.return(vao));

  let vertexBuffer = Gl.createBuffer(gl);

  Gl.bindBuffer(Gl.getArrayBuffer(gl), vertexBuffer, gl);

  Gl.bufferFloat32Data(
    Gl.getArrayBuffer(gl),
    vertices,
    Gl.getStaticDraw(gl),
    gl,
  );

  let positionLocation =
    Shader.GLSLLocation.unsafeGetAttribLocationByNull(
      shaderName,
      "a_position",
      state,
    );
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

  let indexBuffer = Gl.createBuffer(gl);

  Gl.bindBuffer(Gl.getElementArrayBuffer(gl), indexBuffer, gl);

  Gl.bufferUint16Data(
    Gl.getElementArrayBuffer(gl),
    indices,
    Gl.getStaticDraw(gl),
    gl,
  );

  vaoExt##bindVertexArrayOES(. Js.Nullable.null);

  vao;
};

let _initVAOs = (gl, state) => {
  let vaoExt = GPUDetect.unsafeGetVAOExtByThrow(state);

  GameObject.getAllGameObjectData(state)
  |> List.map(
       ({transformData, geometryData, materialData} as gameObjectData) =>
       {
         ...gameObjectData,
         geometryData:
           switch (geometryData.vao) {
           | None =>
             _createVAOs(
               (
                 geometryData.vertices |> GeometryPoints.Vertices.value,
                 geometryData.indices |> GeometryPoints.Indices.value,
               ),
               GameObject.Material.getShaderName(materialData)
               |> ShaderWT.ShaderName.value,
               gl,
               vaoExt,
               state,
             )
             |> GameObject.Geometry.setVAO(_, geometryData)
           | _ => geometryData
           },
       }
     )
  |> GameObject.setAllGameObjectData(_, state);
};

let _getProgram = (shaderName, state) =>
  Shader.Program.unsafeGetProgramByNull(shaderName, state);

let _changeAllGameObjectDataToRenderDataList = (allGameObjectData, gl, state) =>
  allGameObjectData
  |> List.map(
       ({transformData, geometryData, materialData} as gameObjectData) => {
       let shaderName =
         GameObject.Material.getShaderName(materialData)
         |> ShaderWT.ShaderName.value;

       {
         mMatrix:
           GameObject.Transform.getMMatrix(transformData)
           |> CoordinateTransformationMatrix.Model.getMatrixValue,
         vao: GameObject.Geometry.unsafeGetVAOByThrow(geometryData),
         indexCount:
           GameObject.Geometry.getIndices(geometryData)
           |> GeometryPoints.Indices.length,
         colors:
           GameObject.Material.getColors(materialData)
           |> List.map(color => color |> Color.Color3.value),
         program: _getProgram(shaderName, state),
         shaderName,
       };
     });

let _sendAttributeData = (vao, state) =>
  switch (Shader.GLSLSender.getLastSendedVAO(state)) {
  | Some(lastSendedVAO) when lastSendedVAO === vao => state
  | lastSendedVAOpt =>
    GPUDetect.unsafeGetVAOExtByThrow(state)##bindVertexArrayOES(
      Js.Nullable.return(vao),
    );

    Shader.GLSLSender.setLastSendedVAO(vao, state);
  };

let _sendCameraUniformData =
    ((vMatrix, pMatrix), program, shaderName, gl, state) => {
  let vMatrixLocation =
    Shader.GLSLLocation.unsafeGetUniformLocationByNull(
      shaderName,
      "u_vMatrix",
      state,
    );
  let pMatrixLocation =
    Shader.GLSLLocation.unsafeGetUniformLocationByNull(
      shaderName,
      "u_pMatrix",
      state,
    );

  Gl.uniformMatrix4fv(vMatrixLocation, false, vMatrix, gl);
  Gl.uniformMatrix4fv(pMatrixLocation, false, pMatrix, gl);
};

let _sendModelUniformData =
    ((mMatrix, colors), program, shaderName, gl, state) => {
  let mMatrixLocation =
    Shader.GLSLLocation.unsafeGetUniformLocationByNull(
      shaderName,
      "u_mMatrix",
      state,
    );

  Gl.uniformMatrix4fv(mMatrixLocation, false, mMatrix, gl);

  let (state, _) =
    colors
    |> List.fold_left(
         ((state, index), (r, g, b)) => {
           let colorFieldName = {j|u_color$index|j};
           let colorLocation =
             Shader.GLSLLocation.unsafeGetUniformLocationByNull(
               shaderName,
               colorFieldName,
               state,
             );

           (
             Shader.GLSLSender.setShaderCacheMap(
               shaderName,
               Shader.GLSLSender.unsafeGetShaderCacheMapByNull(shaderName, state)
               |> Shader.GLSLSender.sendFloat3(
                    gl,
                    (colorFieldName, colorLocation),
                    [|r, g, b|],
                  ),
               state,
             ),
             index |> succ,
           );
         },
         (state, 0),
       );

  state;
};

let _sendUniformShaderData = (gl, state) => {
  let (vMatrix, pMatrix) = (
    Camera.unsafeGetVMatrixByThrow(state),
    Camera.unsafeGetPMatrixByThrow(state),
  );
  let (vMatrix, pMatrix) = (
    vMatrix |> CoordinateTransformationMatrix.View.getMatrixValue,
    pMatrix |> CoordinateTransformationMatrix.Projection.getMatrixValue,
  );

  Shader.GLSL.getAllValidGLSLEntryList(state)
  |> List.fold_left(
       (state, (shaderName, _)) => {
         let program = Shader.Program.unsafeGetProgramByNull(shaderName, state);

         let state = Shader.Program.use(gl, program, state);

         _sendCameraUniformData(
           (vMatrix, pMatrix),
           program,
           shaderName,
           gl,
           state,
         );

         state;
       },
       state,
     );
};

let render = (gl, state) => {
  DeviceManager.initGlState(gl);

  let state = _initVAOs(gl, state);

  let state = _sendUniformShaderData(gl, state);

  _changeAllGameObjectDataToRenderDataList(
    GameObject.getAllGameObjectData(state),
    gl,
    state,
  )
  |> Result.tryCatch(renderDataList =>
       renderDataList
       |> List.fold_left(
            (state, {mMatrix, vao, indexCount, colors, program, shaderName}) => {
              let state = Shader.Program.use(gl, program, state);

              let state =
                state
                |> _sendModelUniformData(
                     (mMatrix, colors),
                     program,
                     shaderName,
                     gl,
                   );

              let state = _sendAttributeData(vao, state);

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
          )
     );
};