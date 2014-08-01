#include <Galaxy-Log/log.hpp>

#include <neb/timer/Actor/Release.hpp>
#include <neb/gfx/window/Base.hh>
#include <neb/debug.hh>

#include <neb/phx/filter.hpp>
#include <neb/phx/app/base.hpp>
#include <neb/phx/core/actor/rigiddynamic/base.hpp>
#include <neb/phx/core/scene/base.hpp>
#include <neb/phx/core/shape/box.hpp>
#include <neb/phx/game/weapon/SimpleProjectile.hpp>


neb::phx::game::weapon::SimpleProjectile::SimpleProjectile() {
}
void			neb::phx::game::weapon::SimpleProjectile::connect(sp::shared_ptr<neb::gfx::window::base> window) {
	
	auto self(sp::dynamic_pointer_cast<neb::phx::game::weapon::SimpleProjectile>(shared_from_this()));
	
	window->sig_.key_fun_.connect(
			20,
			neb::gfx::window::signals::KeyFun::slot_type(
				&phx::game::weapon::SimpleProjectile::key_fun,
				self.get(),
				_1,
				_2,
				_3,
				_4,
				_5
				).track_foreign(self)
			);

}
int			neb::phx::game::weapon::SimpleProjectile::key_fun(sp::shared_ptr<neb::gfx::window::base> window, int key, int , int action, int mods) {
	if(DEBUG_NEB) BOOST_LOG_CHANNEL_SEV(lg, "phx game weapon", debug) << __PRETTY_FUNCTION__;;

	int key_fire = GLFW_KEY_SPACE;

	switch(action) {
		case GLFW_PRESS:
			if(key == key_fire) {
				fire();
				return 1;
			}
			break;
	}

	return 0;
}
void			neb::phx::game::weapon::SimpleProjectile::fire() {
	if(DEBUG_NEB) BOOST_LOG_CHANNEL_SEV(lg, "phx game weapon", debug) << __PRETTY_FUNCTION__;;
	
	auto app(neb::phx::app::base::global());

	if((app->ts_.time - last_) < cooldown_) return;
	last_ = app->ts_.time;
	
	// create projectile actor

	auto actor(actor_.lock());
	assert(actor);

	auto scene = actor->getPxParent()->getScene();

	auto proj = ::std::dynamic_pointer_cast<neb::phx::core::actor::rigiddynamic::base>(scene->createActorRigidDynamicUninitialized().lock());
	assert(proj);

	scene->insert(proj);

	proj->simulation_.word0 = neb::phx::filter::filter::type::DYNAMIC | neb::phx::filter::filter::type::PROJECTILE;
	proj->simulation_.word1 = neb::phx::filter::filter::RIGID_AGAINST;
	proj->simulation_.word2 = neb::phx::filter::filter::type::PROJECTILE;
	proj->simulation_.word3 = neb::phx::filter::filter::PROJECTILE_AGAINST;






	// initialize position and velocity

	// relative values

	vec3 pos_relative(0,0,-2);
	
	vec3 vel_relative(0,0,-1);
	vel_relative *= velocity_;

	// rotate relative values to actor's space
	pos_relative = actor->pose_.rot_ * pos_relative;
	vel_relative = actor->pose_.rot_ * vel_relative;

	// center on actor
	proj->pose_ = actor->pose_;

	// translate

	proj->pose_.pos_ += vec4(pos_relative,0);

	// velocity

	proj->velocity_ = vel_relative;

	auto rigidbody(actor->isPxActorRigidBodyBase());
	if(rigidbody) {
		proj->velocity_ += rigidbody->velocity_;
	}








	proj->init();

	// shape
	auto shape = proj->createShapeBox(vec3(size_));

	proj->setupFiltering();



	// release timer
	
	auto t = sp::make_shared<neb::Timer::actor::Release>(proj, scene->last_ + 5.0);

	if(DEBUG_NEB) BOOST_LOG_CHANNEL_SEV(lg, "neb timer", debug) << t.use_count();
	
	t->activate();

	if(DEBUG_NEB) BOOST_LOG_CHANNEL_SEV(lg, "neb timer", debug) << t.use_count();
	
	
}

