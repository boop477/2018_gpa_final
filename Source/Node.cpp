# include "../Include/Node.hpp"
# include "../Include/GLM/glm/gtx/quaternion.hpp"
# include "../Include/Common.h"
/* Constructor */
Node::Node(GLuint program, GLint um4p, GLint um4mv, glm::vec3 position = glm::vec3(), glm::quat quaternion = glm::quat()){
    this->_shape = Shape();
    this->_position = position;
    this->_quaternion = quaternion;
    this->_parent = NULL;
    this->_child = NULL;
    this->_sibling = NULL;
    this->_program = program;
    this->_um4p = um4p;
    this->_um4mv = um4mv;
}
/* Add a child to this node. */
void Node::addChild(Node* child){
    if(this->_child == NULL){
        // This node has no child.
        this->_child = child;
    }
    else{
        Node* last_sibling = this->_child;
        // Traverse to the last sibling
        while(last_sibling->hasSibling()){
            last_sibling = last_sibling->getSibling();
        }
        // Assign the new child to the last sibling of the child
        last_sibling->addChild(child);
    }
}
void Node::addParent(Node* parent){this->_parent = parent;}
void Node::addSibling(Node* sibling){this->_sibling = sibling;}

bool Node::hasSibling(){return (this->_sibling == NULL) == false;}
bool Node::hasChild(){return (this->_child == NULL) == false;}

Node* Node::getParent(){return this->_parent;}
Node* Node::getSibling(){return this->_sibling;}
Node* Node::getChild(){return this->_child;}
/* Load a obj file to this node */
void Node::loadObj(std::string filename){
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    
    std::string err;
    
    bool ret = tinyobj::LoadObj(shapes, materials, err, filename.c_str());
    if(err.size()>0)
    {
        printf("Load Models Fail! Please check the solution path");
        return;
    }
    
    printf("Load Models Success ! Shapes size %d Maerial size %d\n", shapes.size(), materials.size());
    
    for(int i = 0; i < shapes.size(); i++)
    {
        /* vertex.vd.glsl
         layout(location = 0) in vec3 iv3vertex;
         layout(location = 1) in vec2 iv2tex_coord;
         layout(location = 2) in vec3 iv3normal;
         */
        glGenVertexArrays(1, &(this->_shape.vao)); // Generate one VAO and assign to _shape.vao
        glBindVertexArray(this->_shape.vao); // Bind thie VAO
        
        glGenBuffers(3, &(this->_shape.vbo)); // Generate 3 VBO(?) and assign to _shape.vbo
        glGenBuffers(1, &(this->_shape.p_normal)); // ??
        glBindBuffer(GL_ARRAY_BUFFER, this->_shape.vbo); // Bind vbo
        glBufferData(GL_ARRAY_BUFFER, shapes[i].mesh.positions.size() * sizeof(float)+shapes[i].mesh.normals.size() * sizeof(float), NULL, GL_STATIC_DRAW); // Register memory address (target, buffer size, pointer to data, usage)
        
        glBufferSubData(GL_ARRAY_BUFFER, 0, shapes[i].mesh.positions.size() * sizeof(float), &shapes[i].mesh.positions[0]);
        glBufferSubData(GL_ARRAY_BUFFER, shapes[i].mesh.positions.size() * sizeof(float), shapes[i].mesh.normals.size() * sizeof(float), &shapes[i].mesh.normals[0]); // Assign data to vbo(target, offset in vbo, data size, pointer to data)
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // How to map data in vbo to vertex attribute specified in vertex shader program.
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *) (shapes[i].mesh.positions.size() * sizeof(float)));
        
        glBindBuffer(GL_ARRAY_BUFFER, this->_shape.p_normal); // Bind p_normal
        glBufferData(GL_ARRAY_BUFFER, shapes[i].mesh.normals.size() * sizeof(float), shapes[i].mesh.normals.data(), GL_STATIC_DRAW); // Register memory address
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0); // How to map data in p_normal to vertex attribute specified in vertex shader program.
        
        glBindBuffer(GL_ARRAY_BUFFER, this->_shape.vboTex); // Bind vboTex(for texture)
        glBufferData(GL_ARRAY_BUFFER, shapes[i].mesh.texcoords.size() * sizeof(float), shapes[i].mesh.texcoords.data(), GL_STATIC_DRAW); // Register memory
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0); // Mapping
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_shape.ebo); // Bind ebo
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[i].mesh.indices.size() * sizeof(unsigned int), shapes[i].mesh.indices.data(), GL_STATIC_DRAW); // Register memory
        this->_shape.materialId = shapes[i].mesh.material_ids[0]; // ?
        this->_shape.indexCount = shapes[i].mesh.indices.size();
        
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
    }
    
    texture_data tdata = load_png("ladybug_diff.png");
    
    glGenTextures( 1, &(this->_shape.m_texture) );
    glBindTexture( GL_TEXTURE_2D, this->_shape.m_texture);
    
    
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32F, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}
/* Get parent_model_mat * tranation_mat * rotation_mat */
glm::mat4 Node::getModelMatrix(){
    // Translation matrix
    glm::mat4 translation_matrix = glm::translate(glm::mat4(), this->_position);
    // Rotation matrix
    glm::mat4 rotation_matrix = glm::toMat4(this->_quaternion);
    // parent * translation * rotation
    if (this->_parent)
        return this->_parent->getModelMatrix() * translation_matrix * rotation_matrix;
    else
        return translation_matrix * rotation_matrix;
}
/* Draw the tree(traverse recursively) */
void Node::drawTree(glm::mat4 view, glm::mat4 projection){
    // Draw myself
    this->_draw(view, projection);
    // Draw siblings
    if (this->_sibling){
        this->_sibling->_draw(view, projection);
    }
    // Draw children
    if (this->_child){
        this->_child->_draw(view, projection);
    }
}
/* Draw ->THIS<- node! */
void Node::_draw(glm::mat4 view, glm::mat4 projection){
    // param:glm::Uniforms uniforms
    // Clear display buffer
    //      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Bind a vertex array for OpenGL (OpenGL will apply operation only to the vertex array objects it bind)
    //      glBindVertexArray(m_shpae.vao);
    glBindVertexArray(this->_shape.vao);
    
    // Tell openGL to use the shader program we created before
    glUseProgram(this->_program);
    
    
    //      GLfloat move = glutGet(GLUT_ELAPSED_TIME) / 50.0;
    //      glm::mat4 translation_matrix = glm::translate(glm::mat4(), temp);
    //      glm::mat4 rotation_matrix = glm::rotate(glm::mat4(), radians(move), glm::vec3(0, 1, 0));
    //      model = translation_matrix*rotation_matrix;
    glm::mat4 model = this->getModelMatrix();
    
    // Transfer value of (view*model) to both shader's inner variable 'um4mv';
    // view is set in My_Reshape()
    glUniformMatrix4fv(this->_um4mv, 1, GL_FALSE, value_ptr(view * model)); // view * (translation * rotation)
    
    // Transfer value of projection to both shader's inner variable 'um4p';
    // projection is set in My_Reshape()
    glUniformMatrix4fv(this->_um4p, 1, GL_FALSE, value_ptr(projection));
    
    // Tell openGL to draw the vertex array we had binded before
    glDrawElements(GL_TRIANGLES, this->_shape.indexCount, GL_UNSIGNED_INT, 0);
    
    // Change current display buffer to another one (refresh frame)
    //      glutSwapBuffers();
}
