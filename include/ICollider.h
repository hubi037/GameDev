 #ifndef __ICollider_h_
 #define __ICollider_h_

/// Interface for objects which need to be informed after a collision happend.
class ICollider
{
public:
	virtual ~ICollider() {}

	/// Called if another object collides with this object.
	/// collider: the other object of the collision.
    virtual void onCollision(ICollider* collider) = 0;
};

#endif