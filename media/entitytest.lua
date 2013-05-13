function update(deltaTime, entity)
	entity:getParentNode():roll(Math.Radian(0.5 * deltaTime), 0);
	entity:getParentNode():pitch(Math.Radian(0.5 * deltaTime), 0);
end