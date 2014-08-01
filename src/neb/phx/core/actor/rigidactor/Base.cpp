#include <Galaxy-Log/log.hpp>
#include <neb/debug.hh>

#include <neb/phx/util/convert.hpp>
#include <neb/phx/core/actor/util/parent.hpp>
#include <neb/phx/core/actor/rigidactor/base.hpp>

#include <neb/phx/core/shape/box.hpp>

//neb::core::actor::rigidactor::base::base() {
//}
neb::phx::core::actor::rigidactor::base::base(sp::shared_ptr<neb::phx::core::actor::util::parent> parent):
	neb::core::actor::base(parent),
	neb::phx::core::actor::base(parent),
	neb::phx::core::actor::actor::base(parent)
{
}
void		neb::phx::core::actor::rigidactor::base::setupFiltering() {
	assert(px_actor_);

	physx::PxRigidActor* actor = (physx::PxRigidActor*)px_actor_;

	physx::PxFilterData coll_data;
	coll_data.word0 = simulation_.word0;
	coll_data.word1 = simulation_.word1;
	coll_data.word2 = simulation_.word2;
	coll_data.word3 = simulation_.word3;

	physx::PxFilterData sq_data;
	sq_data.word0 = scene_query_.word0;
	sq_data.word1 = scene_query_.word1;
	sq_data.word2 = scene_query_.word2;
	sq_data.word3 = scene_query_.word3;


	const physx::PxU32 numShapes = actor->getNbShapes();

	physx::PxShape** shapes = new physx::PxShape*[numShapes];

	actor->getShapes(shapes, numShapes);

	for(physx::PxU32 i = 0; i < numShapes; i++)
	{
		physx::PxShape* shape = shapes[i];
		shape->setSimulationFilterData(coll_data);
		shape->setQueryFilterData(sq_data);
	}

	delete[] shapes;
}
void								neb::phx::core::actor::rigidactor::base::setGlobalPosition(vec3 p) {
	/** @todo if is nested actor, then this is wrong... */
	pose_.pos_ = vec4(p,1);
	
	assert(px_actor_);
	auto px_rigidactor = px_actor_->isRigidActor();
	assert(px_rigidactor);
	px_rigidactor->setGlobalPose(physx::PxTransform(
				neb::phx::util::convert(p),
				neb::phx::util::convert(pose_.rot_)
				));
}




