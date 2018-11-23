#include "OBJObject.h"


void Transform::update(float scale)
{
    rescale(scale);
}

void Transform::spin(glm::vec3 w)
{
    // If you haven't figured it out from the last project, this is how you fix spin's behavior
    //    toWorld = toWorld * glm::rotate(glm::mat4(1.0f), 1.0f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
    GLfloat angle = glm::length(w);
    if(angle<0.0) angle *= -1.0f;
    MT = glm::rotate(glm::mat4(1.0f), angle, glm::normalize(w)) * MT;
}

void Transform::rescale(float scale){
    MT = glm::scale(glm::mat4(1.0f), glm::vec3(scale)) * MT;
}


Geometry::Geometry(const char *filepath)
{
	//toWorld = glm::mat4(1.0f);
	parse(filepath);
    
    // Create array object and buffers. Remember to delete your buffers when the object is destroyed!
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
    // Consider the VAO as a container for all your buffers.
    glBindVertexArray(VAO);
    
    // Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
    // you want to draw, such as vertices, normals, colors, etc.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
    // the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*8*sizeof(GLfloat), vertices_data, GL_STATIC_DRAW);
    // Enable the usage of layout location 0 (check the vertex shader to see what this is)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          8 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          8 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)(3 * sizeof(GLfloat))); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          2, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          8 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)(6 * sizeof(GLfloat))); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    
    // We've sent the vertex data over to OpenGL, but there's still something missing.
    // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*3*sizeof(GLuint), indices_data, GL_STATIC_DRAW);
    
    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind the VAO now so we don't accidentally tamper with it.
    // NOTE: You must NEVER unbind the element array buffer associated with a VAO!
    glBindVertexArray(0);
    
    loadTexture();
}

Geometry::~Geometry()
{
    // Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a
    // large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


void Geometry::parse(const char *filepath)
{
    FILE* fp;
    float x,y,z;
    unsigned int v1, v2, v3, v4, v5, v6, v7, v8, v9;
    fp = fopen(filepath, "rb");
    if (fp==NULL) { std::cerr << "error loading file" << std::endl; exit(-1); }
    
    char buf[256];
    while(fgets(buf, 256, fp)){
        if(buf[0]==0 || buf[1]==0)
            continue;
        if ((buf[0]=='v') && (buf[1]==' ')){
            sscanf(buf, "v %f %f %f", &x, &y, &z);
            tmp_vertices.emplace_back(x, y, z);
        }else if ((buf[0]=='v') && (buf[1]=='n')){
            sscanf(buf, "vn %f %f %f", &x, &y, &z);
            tmp_normals.emplace_back(x, y, z);
        }else if ((buf[0]=='v') && (buf[1]=='t')){
            sscanf(buf, "vt %f %f", &x, &y);
            tmp_uvs.emplace_back(x, y);
        }else if ((buf[0]=='f') && (buf[1]==' ')){
            sscanf(buf, "f %u/%u/%u %u/%u/%u %u/%u/%u", &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, &v9);
            tmp_indices.emplace_back(std::vector<unsigned int>{v1, v2, v3, v4, v5, v6, v7, v8, v9});
            
        }
    }
    
    for( unsigned int v=0; v<tmp_indices.size(); v+=1 ){
        for ( unsigned int i=0; i<3; i+=1 ){
            unsigned int vertexIndex = tmp_indices[v][3*i+0];
            glm::vec3 vertex = tmp_vertices[ vertexIndex-1 ];
            unsigned int uvIndex = tmp_indices[v][3*i+1];
            glm::vec2 uv = tmp_uvs[ uvIndex-1 ];
            unsigned int normalIndex = tmp_indices[v][3*i+2];
            glm::vec3 normal = tmp_normals[ normalIndex-1 ];
            vertices.emplace_back(vertex);
            uvs.emplace_back(uv);
            normals.emplace_back(normal);
        }
        std::vector<unsigned int> index = {3*v, 3*v, 3*v,  3*v+1, 3*v+1, 3*v+1,  3*v+2, 3*v+2, 3*v+2};
        indices.emplace_back(index);
    }
    
    //glm::vec3 max = vertices[0];
    //glm::vec3 min = vertices[0];
    //for( std::vector<glm::vec3>::iterator it=vertices.begin(); it!=vertices.end(); it++){
    //    if(it->x > max.x) max.x = it->x;
    //    if(it->y > max.y) max.y = it->y;
    //    if(it->z > max.z) max.z = it->z;
    //    if(it->x < min.x) min.x = it->x;
    //    if(it->y < min.y) min.y = it->y;
    //    if(it->z < min.z) min.z = it->z;
    //}
    //glm::vec3 offset {(max.x + min.x)/2.0, (max.y + min.y)/2.0, (max.z + min.z)/2.0};
    //GLfloat scale = 10.0/(max.x - min.x);
    glm::vec3 offset {0.0f, 0.0f, 0.0f};
    GLfloat scale {1.0f};

    for(int i=0; i<vertices.size(); i++){
        vertices_data[i][0] = (vertices[i].x - offset.x)*scale;
        vertices_data[i][1] = (vertices[i].y - offset.y)*scale;
        vertices_data[i][2] = (vertices[i].z - offset.z)*scale;
        vertices_data[i][3] = normals[i].x;
        vertices_data[i][4] = normals[i].y;
        vertices_data[i][5] = normals[i].z;
        vertices_data[i][6] = uvs[i].x;
        vertices_data[i][7] = uvs[i].y;
    }
    for(int i=0; i<indices.size(); i++){
        indices_data[i][0] = indices[i][0];
        indices_data[i][1] = indices[i][3];
        indices_data[i][2] = indices[i][6];
    }
    
    fclose(fp);
}

//void OBJObject::draw(float scale, glm::vec3 offset)
void Geometry::draw(GLint shaderProgram, glm::mat4 M)
{
    //// Calculate the combination of the model and view (camera inverse) matrices
    //glm::mat4 modelview = Window::V * toWorld;
    //
    //// We need to calculate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
    //// Consequently, we need to forward the projection, view, and model matrices to the shader programs
    //// Get the location of the uniform variables "projection" and "modelview"
    //uProjection = glGetUniformLocation(shaderProgram, "projection");
    //uModelview = glGetUniformLocation(shaderProgram, "modelview");
    //// Now send these values to the shader program
    //glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    //glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
    //
    //// Now draw the object. We simply need to bind the VAO associated with it.
    //glBindVertexArray(VAO);
    //// Tell OpenGL to draw with triangles, using the number of indices, the type of the indices, and the offset to start from
    //glDrawElements(GL_TRIANGLES, indices.size()*3, GL_UNSIGNED_INT, 0);
    //// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
    //glBindVertexArray(0);
    
    setModelMatrix(M);
    render(shaderProgram);
}



void Geometry::setModelMatrix(glm::mat4 M){
    modelview = Window::V * M;
}

void Geometry::render(GLint shaderProgram){
    
    GLint loc;
    loc = glGetUniformLocation(shaderProgram, "objectColor");
    glUniform3f(loc, color.x, color.y, color.z);
    loc = glGetUniformLocation(shaderProgram, "ambientStrength");
    glUniform1f(loc, ambientStrength);
    loc = glGetUniformLocation(shaderProgram, "diffuseStrength");
    glUniform1f(loc, diffuseStrength);
    loc = glGetUniformLocation(shaderProgram, "specularStrength");
    glUniform1f(loc, specularStrength);
    
    
    // We need to calculate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
    // Consequently, we need to forward the projection, view, and model matrices to the shader programs
    // Get the location of the uniform variables "projection" and "modelview"
    uProjection = glGetUniformLocation(shaderProgram, "projection");
    uModelview = glGetUniformLocation(shaderProgram, "modelview");
    // Now send these values to the shader program
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
    
    // Now draw the object. We simply need to bind the VAO associated with it.
    glBindVertexArray(VAO);
    // Tell OpenGL to draw with triangles, using the number of indices, the type of the indices, and the offset to start from
    glDrawElements(GL_TRIANGLES, indices.size()*3, GL_UNSIGNED_INT, 0);
    // Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
    glBindVertexArray(0);
}


//sasaki3
unsigned char* Geometry::loadPPM(const char* filename, int& width, int& height)
{
    const int BUFSIZE = 128;
    FILE* fp;
    unsigned int read;
    unsigned char* rawData;
    char buf[3][BUFSIZE];
    char* retval_fgets;
    size_t retval_sscanf;
    
    if ( (fp=fopen(filename, "rb")) == NULL)
    {
        std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
        width = 0;
        height = 0;
        return NULL;
    }
    
    // Read magic number:
    retval_fgets = fgets(buf[0], BUFSIZE, fp);
    
    // Read width and height:
    do
    {
        retval_fgets=fgets(buf[0], BUFSIZE, fp);
    } while (buf[0][0] == '#');
    retval_sscanf=sscanf(buf[0], "%s %s", buf[1], buf[2]);
    width  = atoi(buf[1]);
    height = atoi(buf[2]);
    
    // Read maxval:
    do
    {
        retval_fgets=fgets(buf[0], BUFSIZE, fp);
    } while (buf[0][0] == '#');
    
    // Read image data:
    rawData = new unsigned char[width * height * 3];
    read = fread(rawData, width * height * 3, 1, fp);
    fclose(fp);
    if (read != 1)
    {
        std::cerr << "error parsing ppm file, incomplete data" << std::endl;
        delete[] rawData;
        width = 0;
        height = 0;
        return NULL;
    }
    
    return rawData;
}

// load image file into texture object
void Geometry::loadTexture()
{
    //GLuint texture[1];     // storage for one texture
    int twidth, theight;   // texture width/height [pixels]
    unsigned char* tdata;  // texture pixel data
    
    // Load image file
    tdata = loadPPM("Woodland-Camo.ppm", twidth, theight);
    if (tdata==NULL) return;
    
    // Create ID for texture
    glGenTextures(1, &texture[0]);
    
    // Set this texture to be the one we are working with
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    
    // Generate the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata);
    
    // Set bi-linear filtering for both minification and magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

