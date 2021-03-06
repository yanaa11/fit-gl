#include "FirstRenderable.hpp"

#include <QDataStream>
#include <QDebug>
#include <QFile>

#include <GLUtil.hpp>

namespace Kononov {

std::unique_ptr<QOpenGLBuffer>
FirstRenderable::readGLBuffer(QDataStream &stream, QOpenGLBuffer::Type type) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg, hicpp-vararg)
  char *data = nullptr;
  uint size = 0;
  stream.readBytes(data, size);
  auto buffer = bufferFromData(data, size, type);
  // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
  delete[] data;

  return buffer;
}

std::unique_ptr<QOpenGLBuffer>
FirstRenderable::bufferFromData(const char *data, int size,
                                QOpenGLBuffer::Type type) {
  auto buffer = std::make_unique<QOpenGLBuffer>(type);
  buffer->create();
  buffer->bind();
  buffer->allocate(data, size);
  buffer->release();
  return buffer;
}

void FirstRenderable::initVao() {
  const size_t stride = sizeof(GLfloat) * 8;
  const size_t pos_offset = sizeof(GLfloat) * 0;
  const size_t normal_offset = sizeof(GLfloat) * 3;
  const size_t uv_offset = sizeof(GLfloat) * 6;

  /*
   * Create and configure VAO
   */
  m_vao = std::make_unique<QOpenGLVertexArrayObject>();
  m_vao->create();
  m_vao->bind();
  m_vbo->bind();

  // glVertexAttribPointer (specify location of values in vertex structure)
  m_shader->setVertexPositionBuffer(pos_offset, stride);
  m_shader->setVertexNormalBuffer(normal_offset, stride);
  m_shader->setVertexUVBuffer(uv_offset, stride);

  m_vbo->release();
  m_vao->release();
}

FirstRenderable::FirstRenderable(GLenum primitive, const QString &texture_file,
                                 std::shared_ptr<FirstShader> &shader)
    : m_primitive(primitive), m_shader_parameters(), m_shader(shader) {
  m_texture = std::make_unique<QOpenGLTexture>(QImage(texture_file).mirrored());
  m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
  m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
  m_texture->setWrapMode(QOpenGLTexture::Repeat);
}

FirstRenderable::FirstRenderable(GLenum primitive,
                                 std::shared_ptr<FirstShader> &shader,
                                 const QString &texture_file,
                                 const QString &geometry_file)
    : FirstRenderable(primitive, texture_file, shader) {
  QFile file(geometry_file);
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "Unable to open file " << geometry_file;
  }
  QDataStream stream(&file);
  stream.setByteOrder(QDataStream::LittleEndian);
  m_vbo = readGLBuffer(stream, QOpenGLBuffer::VertexBuffer);
  m_ibo = readGLBuffer(stream, QOpenGLBuffer::IndexBuffer);
  initVao();
}

FirstRenderable::FirstRenderable(GLenum primitive,
                                 std::shared_ptr<FirstShader> &shader,
                                 const QString &texture_file,
                                 const char *vbo_data, int vbo_size,
                                 const char *ibo_data, int ibo_size)
    : FirstRenderable(primitive, texture_file, shader) {
  m_vbo = bufferFromData(vbo_data, vbo_size, QOpenGLBuffer::VertexBuffer);
  m_ibo = bufferFromData(ibo_data, ibo_size, QOpenGLBuffer::IndexBuffer);
  initVao();
}

void FirstRenderable::render(QMatrix4x4 view, QMatrix4x4 model) {
  // To render we need to use concrete program, VAO and IBO (VBO is referenced
  // by VAO)
  m_shader->bind();
  m_vao->bind();
  m_ibo->bind();
  m_texture->bind();

  m_shader->setMatrices(view, model);
  m_shader->setParameters(m_shader_parameters);

  // glEnableVertexAttribArray (allow current VAO to affect attribute)
  m_shader->enableAttributeArrays();

  const size_t count = m_ibo->size() / sizeof(GLuint);
  GLUtil::requireFunctions()->glDrawElements(m_primitive, count,
                                             GL_UNSIGNED_INT, nullptr);

  // glDisableVertexAttribArray (because it is not bound to m_program ??? and
  // can affect further rendering)
  m_shader->disableAttributeArrays();

  m_texture->release();
  m_ibo->release();
  m_vao->release();
  m_shader->release();
}

const FirstShaderParameters &
FirstRenderable::getShaderParameters() const noexcept {
  return m_shader_parameters;
}

FirstShaderParameters &FirstRenderable::getShaderParameters() noexcept {
  return m_shader_parameters;
}

void FirstRenderable::setShaderParameters(
    const FirstShaderParameters &shader_parameters) {
  m_shader_parameters = shader_parameters;
}

} // namespace Kononov
