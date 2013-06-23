function update(position, target)
	return (target - position)	
end

function updateState(health, state)
-- states 0: PURSUE, 1: FLEE, 2: WANDER
	if state == 0 and health < 0.25 then return 1 end
	if state == 1 and health > 0.75 then return 0 end
	
	return state
end