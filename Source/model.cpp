#include "model.hpp"
#include "vertexData.h"
#include "textureData.h"

#include "../Include/STB/stb_image.h"

texture_data load_png(const char* const pngFilepath);

Model::Model(std::string filename, std::string prefix, glm::vec3 position = glm::vec3(), glm::quat quaternion = glm::quat(), glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0)){
    this->_position = position;
    this->_quaternion = quaternion;
    this->_scale = scale;
	this->_prefix = prefix;

	//TCHAR pwd[256];
	//GetCurrentDirectory(256, pwd);
	//MessageBox(NULL, pwd, pwd, 0);

    // Use assimp to read scene
    //Assimp::Importer importer;
	const aiScene* scene = aiImportFile(filename.c_str(), aiProcessPreset_TargetRealtime_Fast);
	if (scene == nullptr) {
		//std::cout << importer.GetErrorString() << std::endl;
		std::cout << "scene is a nullptr" << std::endl;
	}
	else {
		std::cout << "Model.cpp: Read scene" << std::endl;
	}
	//const aiScene* scene = aiImportFile(filename.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	// parse the tree
    parseTree(scene->mRootNode, scene);
	aiReleaseImport(scene);
    
    //tex_ID = this->getTextureID("vokselia_spawn/vokselia_spawn.png"); // 2nd
    tex_ID = this->getTextureID("lost_empire/lost_empire-RGBA.png"); // 2nd
    if (tex_ID == GLuint(99)) {
        printf("My_Display::texture not found\n");
    }
    
	std::cout << "Model.cpp: Parsing done, release scene" << std::endl;
}
// Diffuse map + texture
void Model::draw(UniformList uniform_list,
                glm::mat4 view_matrix,
                glm::mat4 proj_matrix,
                glm::mat4 light_vp_matrix,
                bool is_shadow){
    /*
     * Should invoke glUseProgram before calling this function.
     * Use this function to draw a model with environment mapping, shadow and BF lighting.
     */
    glm::mat4 model = getModelMatrix();
    glm::mat4 light_mvp_matrix = light_vp_matrix * model;
    glUniformMatrix4fv(uniform_list.render.model_matrix, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(uniform_list.render.view_matrix, 1, GL_FALSE, &view_matrix[0][0]);
    glUniformMatrix4fv(uniform_list.render.proj_matrix, 1, GL_FALSE, &proj_matrix[0][0]);
    glUniformMatrix4fv(uniform_list.render.light_mvp_matrix, 1, GL_FALSE, &light_mvp_matrix[0][0]);
    //glUniform1i(uniform_list.render.is_quad, 0);
    if(is_shadow){
        glUniform1i(uniform_list.render.is_shadow, 1);
    }
    else{
        glUniform1i(uniform_list.render.is_shadow, 0);
    }
    
    for(int i = 0; i < _meshes.size(); i++){
        //for (int i = 0; i < 1; i++) {
        _meshes[i].draw();
    }
    
    glUniform1i(uniform_list.render.is_quad, 0);
}
// For shadow map from light view
void Model::draw(UniformList uniform_list,
               glm::mat4 light_vp_matrix){
    /*
     * Should invoke glUseProgram before calling this function.
     * Invoke this function to get a depth map from light.
     */
    glm::mat4 model = getModelMatrix();   // Assign uniforms
    glUniformMatrix4fv(uniform_list.depth.mvp, 1, GL_FALSE, value_ptr(light_vp_matrix * model));
    
    for(int i = 0; i < _meshes.size(); i++){
        _meshes[i].draw();
    }
}
// For ssao
void Model::draw(UniformList uniform_list,
                 glm::mat4 view_matrix,
                 glm::mat4 proj_matrix){
    /*
     * Should invoke glUseProgram before calling this function.
     * Invoke this function to get a depth map and a normal map.
     * Those maps will be used to get a ssao map.
     */
    glm::mat4 model = getModelMatrix();
    glm::mat4 mv_matrix = view_matrix * model;
    
    glUniformMatrix4fv(uniform_list.depth_normal.mv_matrix, 1, GL_FALSE, &mv_matrix[0][0]);
    glUniformMatrix4fv(uniform_list.depth_normal.proj_matrix, 1, GL_FALSE, &proj_matrix[0][0]);
    
    for(int i = 0; i < _meshes.size(); i++){
        _meshes[i].draw();
    }
}

void Model::parseTree(aiNode* node, const aiScene* scene){
    // Parse meshes of this node
    for(int i = 0; i < node->mNumMeshes; i++){
        aiMesh* mesh_data = scene->mMeshes[node->mMeshes[i]];
        _meshes.push_back(parseMesh(mesh_data, scene));
    }
    for(int i = 0; i < node->mNumChildren; i++){
        parseTree(node->mChildren[i], scene);
    }
}

Mesh Model::parseMesh(aiMesh* mesh_data, const aiScene* scene){
    /* Parse the mesh_data */
    std::vector<Vertex> vertices;
    /* #(unsigned int mNumVertices) of vertices contain {
           aiVector3D* mVertices
           aiVector3D* mNormals
           aiVector3D* mTextureCoords } in A mesh. */
    for(unsigned int i = 0; i < mesh_data->mNumVertices; i++){
        Vertex v;
        v.cord = glm::vec3(mesh_data->mVertices[i].x,
                             mesh_data->mVertices[i].y,
                             mesh_data->mVertices[i].z);
        v.normal = glm::vec3(mesh_data->mNormals[i].x,
                                   mesh_data->mNormals[i].y,
                                   mesh_data->mNormals[i].z);
        v.tex_cord = glm::vec2(mesh_data->mTextureCoords[0][i].x,
                                     mesh_data->mTextureCoords[0][i].y);
		//printf("%f %f\n", v.tex_cord.x, v.tex_cord.y);
        vertices.push_back(v); 
    }
    /* Drawing order
     #(unsigned int mNumFaces) of faces, each contains #(unsigned int mNumIndices)
     of indices. */
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < mesh_data->mNumFaces; i++){
		for (unsigned int j = 0; j < mesh_data->mFaces[i].mNumIndices; j++) {
			indices.push_back(mesh_data->mFaces[i].mIndices[j]);
		}
    }
    /* Texture (Realllllly don't know how to load this...)
           mMaterialIndex->aiMaterial* scene.mMaterials->GetTexture(type) */
    //std::vector<GLuint> tex_ids;
	GLuint textureID;
    aiMaterial* material = scene->mMaterials[mesh_data->mMaterialIndex]; // Get the material used by this mesh
    // Load diffuse map in this material
	if (material->GetTextureCount(aiTextureType_DIFFUSE) != 1) {
		std::cout << "Texture count != 1" << std::endl;
		// Practice: load ladybug texture start
		std::string path = "Ladybug/ladybug_diff.png";
		texture_data tdata = load_png("Ladybug/ladybug_diff.png");

		//GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Practice: load ladybug texture end
		/* Push the textureID into loaded_tex_path_id (model) */
		_loaded_tex_path_id[path] = textureID;
		/* Push the textureID into tex_ids (mesh) */
		//tex_ids.push_back(textureID);
		std::cout << _loaded_tex_path_id[path] << "(load)" << std::endl;
	}
    for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++){
        aiString name;
        material->GetTexture(aiTextureType_DIFFUSE, i, &name);
		std::string path = _prefix + std::string(name.C_Str());
		// Check if the texture has been loaded
        if (_loaded_tex_path_id.find(path) != _loaded_tex_path_id.end()){
            // path matched, loaded texture
            //tex_ids.push_back(_loaded_tex_path_id[path]);
			textureID = _loaded_tex_path_id[path];
        }
        else{
            /* load the texture */
            int width, height, chn_code;
			
            unsigned char *data = stbi_load(path.c_str(), &width, &height, &chn_code, 0);
			if (data == nullptr) {
				std::cout << "model.cpp: unable to open texture " << path << std::endl;
			}
			else {
				std::cout << "model.cpp: Load " << path << std::endl;
			}
            // Decode channels, see https://github.com/nothings/stb/blob/master/stb_image.h
            // line 147:
            //     N=#comp     components
            //       1           grey
            //       2           grey, alpha
            //       3           red, green, blue
            //       4           red, green, blue, alpha
            GLenum channel = GL_RGBA;
            if (chn_code == 3){
                channel = GL_RGB;
            }
            else if (chn_code == 4){
                channel = GL_RGBA;
            }
            else{
				std::cout << "model.cpp: unable to decode chn_code\n" << std::endl;
                throw "model.cpp: unable to decode chn_code\n";
            }
			texture_data tdata = load_png(path.c_str());

			//GLuint textureID;
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);

			glTexImage2D(GL_TEXTURE_2D, 0, channel, tdata.width, tdata.height, 0, channel, GL_UNSIGNED_BYTE, tdata.data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            /* Push the textureID into loaded_tex_path_id (model) */
            _loaded_tex_path_id[path] = textureID;
            /* Push the textureID into tex_ids (mesh) */
            //tex_ids.push_back(textureID);
			stbi_image_free(data);
			//std::cout << _loaded_tex_path_id[path] << "(load)" << std::endl;
        }
    }
    /* Create and return a mesh */
    return Mesh(vertices, indices, textureID);
}

void Model::log() {
	std::cout << "------------" << std::endl;
	std::cout << _prefix << std::endl;
	std::cout << "#mesh: " << this->_meshes.size() << std::endl;
	std::cout << "#_loaded_tex_path_id: " << _loaded_tex_path_id.size() << std::endl;
	for (std::map<std::string, GLuint>::iterator i = _loaded_tex_path_id.begin();
		i != _loaded_tex_path_id.end(); ++i) {
		std::cout << i->first << " " << i->second << std::endl;
	}
	std::cout << "------------" << std::endl;
}

void Model::addPosition(glm::vec3 position){
    this->_position += position;
}
void Model::setPosition(glm::vec3 position){
    this->_position = position;
}
void Model::addQuaternion(glm::quat quaternion){
    this->_quaternion = this->_quaternion * quaternion;
}
void Model::setQuaternion(glm::quat quaternion){
    this->_quaternion = quaternion;
}
void Model::addScale(glm::vec3 scale){
    this->_scale += scale;
}
void Model::setScale(glm::vec3 scale){
    this->_scale = scale;
}
GLuint Model::getTextureID(std::string path) {
	if (_loaded_tex_path_id.find(path) != _loaded_tex_path_id.end()) {
		// path matched, loaded texture
		return _loaded_tex_path_id[path];
	}
	else {
		return GLuint(99);
	}
}

glm::mat4 Model::getModelMatrix() {
	// Translation matrix
	glm::mat4 translation_matrix = glm::translate(glm::mat4(), _position);
	// Rotation matrix
	glm::mat4 rotation_matrix = glm::toMat4(_quaternion);
	// Scale matrix
	glm::mat4 scale_matrix = glm::scale(_scale);

	return translation_matrix * rotation_matrix * scale_matrix;
}

// load a png image and return a TextureData structure with raw data
// not limited to png format. works with any image format that is RGBA-32bit
texture_data load_png(const char* const pngFilepath)
{
    texture_data texture;
    int components;
    
    // load the texture with stb image, force RGBA (4 components required)
    stbi_uc *data = stbi_load(pngFilepath, &texture.width, &texture.height, &components, 4);
    
    // is the image successfully loaded?
    if (data != NULL)
    {
        // copy the raw data
        size_t dataSize = texture.width * texture.height * 4 * sizeof(unsigned char);
        texture.data = new unsigned char[dataSize];
        memcpy(texture.data, data, dataSize);
        
        // mirror the image vertically to comply with OpenGL convention
        for (size_t i = 0; i < texture.width; ++i)
        {
            for (size_t j = 0; j < texture.height / 2; ++j)
            {
                for (size_t k = 0; k < 4; ++k)
                {
                    size_t coord1 = (j * texture.width + i) * 4 + k;
                    size_t coord2 = ((texture.height - j - 1) * texture.width + i) * 4 + k;
                    std::swap(texture.data[coord1], texture.data[coord2]);
                }
            }
        }
        
        // release the loaded image
        stbi_image_free(data);
    }
    
    return texture;
}
