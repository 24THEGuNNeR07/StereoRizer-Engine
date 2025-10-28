#include <graphics/RenderObject.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace stereorizer::graphics;

RenderObjectData::RenderObjectData(BufferAccessType accessType, BufferCallType callType, DrawType drawType) : accessType(accessType), callType(callType), drawType(drawType)
{
}

RenderObject::RenderObject(uint32_t id, RenderObjectData objectParams, Material *const material) : rObjectID(id), objectParams(objectParams), material(material)
{
}

RenderObject::RenderObject(uint32_t id, const RenderObject &from) : rObjectID(id), objectParams(from.objectParams), material(from.material), vtxBuffer(from.vtxBuffer), vtxArray(from.vtxArray), elementBuffer(from.elementBuffer)
{
}

bool RenderObject::hasElementBuffer() const
{
	return elementBuffer == nullptr;
}

const glm::mat4 &RenderObject::getModel(bool forcedCalculate)
{
	if(!forcedCalculate && !isModelChanged())
		return model;
	model = glm::translate(glm::mat4(1.0f), getPosition());
	model = glm::scale(model, getScale());
	const glm::vec3 &rotation = getRotation();
	model = model * glm::eulerAngleYZX(rotation.x, rotation.y, rotation.z);
	invModel = glm::inverse(model);
	transInvModel = glm::transpose(invModel);
	setChangedModel(false);
	return model;
}

void RenderObject::updateMVP(FreeCamera &camera)
{
	getModel();
	const glm::mat4 &VP = camera.getVPMatrix();
	const glm::mat4 &V = camera.getViewMatrix();
	const glm::mat4 &P = camera.getProjectionMatrix();
	glm::mat4 MVP = VP * model;
	glm::mat4 MV = V * model;

	material->shader.setUniform<glm::mat4>("model.M", model);
	material->shader.setUniform<glm::mat4>("model.IM", invModel);
	material->shader.setUniform<glm::mat4>("model.TIM", transInvModel);

	material->shader.setUniform<glm::mat4>("model.MVP", MVP);
	material->shader.setUniform<glm::mat4>("model.MV", MV);
	if(camera.isVPUpdated()) // TODO update VP in renderer/camera
	{
		material->shader.setUniform("model.P", P);
		material->shader.setUniform("model.VP", VP);
		material->shader.setUniform("model.V", V);
		material->shader.setUniform("camera.viewPos", camera.getCameraPos());
	}
}

void RenderObject::draw(FreeCamera &camera)
{
	material->use();
	draw(camera, material->shader);
}

void RenderObject::draw(FreeCamera &camera, Shader &shader)
{
	shader.useIfNecessary();
	updateMVP(camera);
	if(elementBuffer != nullptr)
	{
		vtxArray->drawElements(*elementBuffer, objectParams.drawType);
		return;
	}
	vtxArray->drawArray(*vtxBuffer, objectParams.drawType);
}

const VertexBuffer *RenderObject::getVertexBuffer()
{
	return vtxBuffer;
}
const VertexArray *RenderObject::getVertexArray()
{
	return vtxArray;
}
const ElementBuffer *RenderObject::getElementBuffer()
{
	return elementBuffer;
}

SimpleRenderObject::SimpleRenderObject(uint32_t id, RenderObjectData objectParams, Material *const material, std::vector<float> vertices, std::vector<uint32_t> attributeSizes) : RenderObject(id, objectParams, material), vertices(vertices), attributeSizes(attributeSizes)
{
}
SimpleRenderObject::SimpleRenderObject(uint32_t id, RenderObjectData objectParams, Material *const material, std::vector<float> vertices, std::vector<uint32_t> attributeSizes, std::vector<uint32_t> indices) : RenderObject(id, objectParams, material), vertices(vertices), attributeSizes(attributeSizes), indices(indices)
{
}
SimpleRenderObject::SimpleRenderObject(uint32_t id, const SimpleRenderObject &from) : RenderObject(id, from), vertices(from.vertices), attributeSizes(from.attributeSizes), indices(from.indices)
{
}
void SimpleRenderObject::initObject()
{
	vtxArray = new VertexArray();
	vtxBuffer = new VertexBuffer(*vtxArray, vertices, attributeSizes, BufferAccessType::STATIC, BufferCallType::DRAW);
	if(indices.size() != 0)
		elementBuffer = new ElementBuffer(indices, BufferAccessType::STATIC, BufferCallType::DRAW);
}

const std::vector<float> &SimpleRenderObject::getVertices()
{
	return vertices;
}
const std::vector<uint32_t> &SimpleRenderObject::getAttributeSizes()
{
	return attributeSizes;
}
const std::vector<uint32_t> &SimpleRenderObject::getIndices()
{
	return indices;
}