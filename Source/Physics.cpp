#include "Physics.h"
#include "Model.h"

Physics::Physics()
{
	InitData();
}

Physics::~Physics()
{
}

void Physics::InitData()
{
	m_pCollisionConfiguration	= NULL;
	m_pCollisionDispatcher		= NULL;
	m_pBroadphaseItf			= NULL;
	m_pSolver					= NULL;
	m_pDynamicsWorld			= NULL;

	m_btObjects.clear();
}

btTransform Physics::glmMat4TobtTransform(const glm::mat4& transform)
{
	glm::mat3 m3(transform);
	return btTransform(btMatrix3x3(m3[0][0],m3[1][0],m3[2][0],m3[0][1],m3[1][1],m3[2][1],m3[0][2],m3[1][2],m3[2][2]), btVector3(transform[3][0], transform[3][1], transform[3][2]));
}

glm::mat4 Physics::btTransformToGlmMat4(const btTransform& transform)
{
	glm::mat4 glmMat4;
	transform.getOpenGLMatrix(glm::value_ptr(glmMat4));
	return glmMat4;
}

void Physics::Initialize()
{
	int bulletLibVersion = btGetVersion();
	ConsoleWriteOk("Bullet (physic library) :");
	ConsoleWrite  (" -> version : %d.%d", bulletLibVersion/100, bulletLibVersion%100);

	m_pCollisionConfiguration	= new btDefaultCollisionConfiguration();
	m_pCollisionDispatcher		= new btCollisionDispatcher(m_pCollisionConfiguration);
	m_pBroadphaseItf			= new btDbvtBroadphase();
	m_pSolver					= new btSequentialImpulseConstraintSolver;
	m_pDynamicsWorld			= new btDiscreteDynamicsWorld(m_pCollisionDispatcher, m_pBroadphaseItf, m_pSolver, m_pCollisionConfiguration);

	m_pDynamicsWorld->setGravity(btVector3(0,-90,0));
}

void Physics::Terminate()
{
	for (auto obj : m_btObjects)
	{
		btRigidBody*		pBody  = obj.first;
		btCollisionShape*	pShape = obj.second.second;
		if (pBody->getMotionState())
		{
			delete pBody->getMotionState();
		}
		m_pDynamicsWorld->removeRigidBody(pBody);
		delete pBody;
		delete pShape;
	}

	delete m_pDynamicsWorld;
	delete m_pSolver;
	delete m_pBroadphaseItf;
	delete m_pCollisionDispatcher;
	delete m_pCollisionConfiguration;

	InitData();
}

btRigidBody* Physics::AddDynamicObjetToPhysic(btCollisionShape* pShape, glm::mat4 transform, glm::vec3 inertia, float mass, float friction)
{
	// Ajoute l'inertie
	btVector3 localInertia(inertia.x,inertia.y,inertia.z);
	pShape->calculateLocalInertia(mass,localInertia);

	// Calcule les matrices bullet
	btTransform btTrans = glmMat4TobtTransform(transform);

	// Construit le rigidBody
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, new btDefaultMotionState(btTrans), pShape, localInertia);
	btRigidBody* pRigidBody = new btRigidBody(rbInfo);
	pRigidBody->setFriction(friction);
	pRigidBody->activate();
	m_pDynamicsWorld->addRigidBody(pRigidBody);

	// Et on stocke l'objet bullet et sa matrice en format glm::mat4
	m_btObjects[pRigidBody] = make_pair(transform,pShape);
	return pRigidBody;
}

btRigidBody* Physics::AddStaticObjetToPhysic(btCollisionShape* pShape, glm::mat4 transform)
{
	// Calcule les matrices bullet
	btTransform btTrans = glmMat4TobtTransform(transform);

	// Construit le rigidBody
	btRigidBody::btRigidBodyConstructionInfo rbInfo(0, new btDefaultMotionState(btTrans), pShape, btVector3(0,0,0));
	btRigidBody* pRigidBody = new btRigidBody(rbInfo);
	pRigidBody->setFriction(0.5f);
	pRigidBody->activate();
	m_pDynamicsWorld->addRigidBody(pRigidBody);

	// Et on stocke l'objet bullet et sa matrice en format glm::mat4
	m_btObjects[pRigidBody] = make_pair(transform,pShape);
	return pRigidBody;
}

// Ajoute un objet dynamique (sphere)
btRigidBody* Physics::AddDynamicSphereToPhysic(float radius, glm::mat4 transform, glm::vec3 inertia, float mass, float friction)
{
	btSphereShape* pShape = new btSphereShape(radius);
	return AddDynamicObjetToPhysic(pShape, transform, inertia, mass, friction);
}

// Ajoute un objet dynamique (enveloppe convexe, c'est une approximation)
btRigidBody* Physics::AddDynamicConvexHullToPhysic(const Model* pModel, glm::mat4 transform, glm::vec3 inertia, float mass, float friction)
{
	btConvexHullShape* pShape = new btConvexHullShape();

	const vector<Mesh>& meshs = pModel->getMeshs();
	for (size_t i=0; i<meshs.size(); i++)
	{
		const vector<Vertex>& vertices = meshs[i].m_vertices;
		for (size_t j=0; j<vertices.size(); j++)
		{
			pShape->addPoint(btVector3(vertices[j].Position.x, vertices[j].Position.y, vertices[j].Position.z));
		}
	}
	return AddDynamicObjetToPhysic(pShape, transform, inertia, mass, friction);
}

// Ajoute un objet fixe (peut être concave ou non)
btRigidBody* Physics::AddStaticConcaveObjetToPhysic(const Model* pModel, glm::mat4 transform)
{
	btTriangleMesh* pTriangleMesh = new btTriangleMesh();

	const vector<Mesh>& meshs = pModel->getMeshs();
	for (size_t i=0; i<meshs.size(); i++)
	{
		const vector<Vertex>& vertices = meshs[i].m_vertices;
		const vector<GLuint>& indices  = meshs[i].m_indices;
		for (size_t j=0; j<indices.size(); )
		{
			GLuint a = indices[j++];
			GLuint b = indices[j++];
			GLuint c = indices[j++];
			pTriangleMesh->addTriangle(	btVector3(vertices[a].Position.x, vertices[a].Position.y, vertices[a].Position.z),
										btVector3(vertices[b].Position.x, vertices[b].Position.y, vertices[b].Position.z),
										btVector3(vertices[c].Position.x, vertices[c].Position.y, vertices[c].Position.z) );
		}
	}
	btBvhTriangleMeshShape* pShape = new btBvhTriangleMeshShape(pTriangleMesh, true); // optimized for static mesh
	return AddStaticObjetToPhysic(pShape,transform);
}

// Ajoute un objet fixe (plan infini)
btRigidBody* Physics::AddStaticPlaneObjetToPhysic()
{
	btStaticPlaneShape* pStaticPlane = new btStaticPlaneShape(btVector3(0,1,0), 0.0f);	// vecteur normal + constante du plan
	return AddStaticObjetToPhysic(pStaticPlane, glm::mat4(1));
}

void Physics::RemoveObject(btRigidBody* pBody)
{
	auto				obj    = m_btObjects.find(pBody);
	btCollisionShape*	pShape = obj->second.second;
	if (pBody->getMotionState())
	{
		delete pBody->getMotionState();
	}
	m_pDynamicsWorld->removeRigidBody(pBody);
	delete pBody;
	delete pShape;

	m_btObjects.erase(pBody);
}

glm::mat4 Physics::GetObjectUpdatedMatrix(btRigidBody* pBody)
{
	return m_btObjects[pBody].first;
}

void Physics::SetObjectMatrix(btRigidBody* pBody, const glm::mat4& mat)
{
	m_btObjects[pBody].first = mat;

	btMotionState* pMs = pBody->getMotionState();
	if (pMs)
	{
		btTransform trans = glmMat4TobtTransform(mat);
		pMs->setWorldTransform(trans);
	}
}

void Physics::UpdatePhysics(float deltaTimeInSecond)
{
	m_pDynamicsWorld->stepSimulation(deltaTimeInSecond,2);	// paramètres : delta de temps et maximum number of sub-step
	//m_pDynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	//m_pDynamicsWorld->debugDrawWorld();
	for (int j=0,jLen=m_pDynamicsWorld->getNumCollisionObjects(); j<jLen; j++)
	{
		btCollisionObject*	pObj  = m_pDynamicsWorld->getCollisionObjectArray()[j];
		btRigidBody*		pBody = btRigidBody::upcast(pObj);

		btTransform trans;
		if (pBody && pBody->getMotionState())
		{
			pBody->getMotionState()->getWorldTransform(trans);
		}
		else
		{
			trans = pObj->getWorldTransform();
		}

		m_btObjects[pBody].first = btTransformToGlmMat4(trans);
	}
}

void Physics::SetVelocity(btRigidBody* pBody, glm::vec3 direction)
{
	pBody->setLinearVelocity(btVector3(direction.x,direction.y,direction.z));
}


