#include <graphics/Renderer.h>

using namespace stereorizer::graphics;

uint32_t RendererData::getClearMask()
{
	return (clearColor * GL_COLOR_BUFFER_BIT) | (clearDepth * GL_DEPTH_BUFFER_BIT);
}

Renderer *Renderer::renderer;
Renderer::Renderer(RendererData data, float aspectRatio) : camera(new FreeCamera(glm::vec3(0.0f), aspectRatio)), data(data), lightManager(new LightManager())
{
	renderer = this;
}

void Renderer::init()
{
	if(data.depthTest)
	{
		glEnable(GL_DEPTH_TEST);
	}
}

SimpleRenderObject *Renderer::addOddObject(RenderObjectData data, Material *material, const std::vector<float> &vertices, const std::vector<uint32_t> &attributeSizes)
{
	uint32_t id = itemCount++;
	SimpleRenderObject *renderObject = new SimpleRenderObject(id, data, material, vertices, attributeSizes);
	addRenderObject(renderObject);
	return renderObject;
}

SimpleRenderObject *Renderer::addSimpleObject(RenderObjectData data, Material *material, const std::vector<float> &vertices, const std::vector<uint32_t> &attributeSizes, const std::vector<uint32_t> &indices)
{
	uint32_t id = itemCount++;
	SimpleRenderObject *renderObject = new SimpleRenderObject(id, data, material, vertices, attributeSizes, indices);
	addRenderObject(renderObject);
	return renderObject;
}

Mesh *Renderer::addMesh(RenderObjectData data, Material *material, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices)
{
	uint32_t id = itemCount++;
	Mesh *mesh = new Mesh(id, data, material, vertices, indices, true);
	addRenderObject(mesh);
	return mesh;
}

AssimpModel *Renderer::addModel(const Material &baseMaterial, const char *path)
{
	return new AssimpModel(path, baseMaterial);
}

void Renderer::draw()
{
	glClearColor(data.resetCol[0], data.resetCol[1], data.resetCol[2], data.resetCol[3]);
	glClear(data.getClearMask());

	camera->updateMatrices();

	lightManager->updateLights();

	for(auto const &it : renderObjects)
	{
		it.second->draw(*camera);
	}

	camera->clearFlags();
}

Renderer::~Renderer()
{
	std::cout << "Renderer deleted." << std::endl;
	delete lightManager;
	delete camera;
	for(auto const &it : renderObjects)
	{
		delete it.second;
	}
	for(auto &it : vertexBuffers)
	{
		delete it;
	}
	for(auto &it : vertexArrays)
	{
		delete it;
	}
	for(auto &it : elementBuffers)
	{
		delete it;
	}

}

void Renderer::addRenderObject(RenderObject *renderObject)
{
	renderObject->initObject();
	vertexBuffers.push_back(renderObject->getVertexBuffer());
	vertexArrays.push_back(renderObject->getVertexArray());
	if(renderObject->hasElementBuffer())
		elementBuffers.push_back(renderObject->getElementBuffer());

	renderObjects[renderObject->rObjectID] = renderObject;
}