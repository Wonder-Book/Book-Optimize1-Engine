# benchmark test: show 7000 triangles



# find problem:
profile
timeline?
memory


# optimize

(optimize each one and compare benchmark)


## optimize initAll

optimize:
get shader info only when debug


why set "isDebug" to state data instead of state?
(and why mutable?)





## optimize loop


- cache location
////utils: add sparse map








- set camera data only once

////(split uniform data to camera data, model data)



- cache uniform data
not all uniform data need cache
(e.g. matrix data not cache; array number(int/float) data can cache)



/*
need test difference perf!!!

add MutableHashMap for performance:
uniformCacheMap, shaderCacheMap is MutableHashMap

*/







- extract shaderIndex
add SparseMap








- store send func when init shader
change glsl->2: add "u_alpha"
need extract glsl sender data!


if not exist , not add to send arr
           AllGLSLLocationService.isUniformLocationExist(pos) ?
           AllGLSLLocationService.isAttribLocationExist(pos) ?



extract render state, sub state?











- cache use program


- use vao(extension)

judge last vao
