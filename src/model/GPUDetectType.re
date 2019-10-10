type vaoExt = {
  .
  [@bs.meth] "createVertexArrayOES": unit => GlType.vao,
  [@bs.meth] "bindVertexArrayOES": Js.Nullable.t(GlType.vao) => unit,
};

external glExtensionToVAOExt: GlType.extension => vaoExt = "%identity";