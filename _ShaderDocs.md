amVK_Pipelines
----
If you wanna change any mod in amVK_GraphicsPipes, do that before calling `amVK_Pipelines::Build_4_Device()`

***SEE: `2D_Image.hh` & `2D_Quad.hh`***
e.g. do `PS.vertTopo = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP`   before that function
then you should revert back to the default value... for best practices....
because we wanna keep that PipeStore... to Default level....
Otherwise we'd have to copy and make multiple copies of GraphicsPipes


VertexInputBinding (VAO)
----
https://vkguide.dev/docs/chapter-3/triangle_mesh_code/
Check out `VIBindings` inside `amVK_Pipeline.hh`

#### IndexBuffer

```
NOTE: You can Also use *IndexBuffer* which is a cool thing.... czzz it can reduce memory usage by 33% .... 
     than TriangleList
      But TriangleStrip reduces by 66% (if its best case scenario... eg. SubDivided Grid/Plane)

e.g.       TriangleList = 3x                (3Vertex = 9Float / perTriangle)
 VertexList+IndexBuffer = 1x+(3/3)x = 2x    (3Index  = 3Float / perTriangle) + VertexList
```

#### 'PrimitiveRestart' (an IndexBuffer Mod)
> 'Primitive restart functionality allows you to tell OpenGL that a particular index value means, not to source a vertex at that index, but to begin a new Primitive of the same type with the next vertex.'
   - from Wikipedia https://www.khronos.org/opengl/wiki/Vertex_Rendering#Primitive_Restart
   - INTERESTING: it is an alternative to `glMultiDrawElements`

</br>
</br>


VertexTopology
----
<table>
  <tbody>
    <tr>
      <td>
        `VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST`:
      </td>
      <td>
        `VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP`:
      </td>
    </tr>
    <tr>
      <td>
        <img src=".readme/V2/TriangleList.png">
      </td>
      <td>
        <img src=".readme/V2/TriangleStrip.png">
      </td>
    </tr>
    <tr>
      <td>
        - <code>gl_VertexIndex</code> starts from zero....
        </br>
        - these 0,1,2,3,4,5.... ties with <code>VertexInputBinding</code> <i>(VAO in GL)</i>
      </td>
      <td>
        <code>gl_VertexIndex</code> will be the same as it would have been with <b><i>TRIANGLE_LIST</i></b> tho....
        </br>
        e.g. <i>(1, 3, 2)</i> triangle will be <i>(3, 4, 5)</i> as <code>gl_VertexIndex</code>
      </td>
    </tr>
  </tbody>
</table>

You can think of this *functionality* or rathermore as I call it a ***fixed-function*** mod.... as like... it comes before the `.vert` ***VertexShader*** stage.... and ***TriangleStrip*** basically is for *MemoryOptimization*
</br>

This is a C++ side stuff... You dont gotta worry/think about this as a Shader Artist....