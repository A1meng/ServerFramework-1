
--ս���ű�ͨ�ú��������ļ�

--�������ͱ���
COMBAT_CAMP_ACTIVE = 1    --����ս����
COMBAT_CAMP_PASSIVE = 2   --����ս����

--����ս���������Եı���
COMBAT_EXTRAATTR_GENDER = 1		--ս����λ���Ա�
COMBAT_EXTRAATTR_GOOD = 2		--�ƶ�ֵ
COMBAT_EXTRAATTR_CONFIGID = 3	--ս����λ���õ�ID

--���弼�ܽű��ĵ�������
SKILL_SCRIPT_CALL_DAMAGE = 1		--�����˺�ʱ����
SKILL_SCRIPT_CALL_SKILLDONE = 2		--���ܴ�����ɺ����
SKILL_SCRIPT_CALL_CHIEFDODGE = 3 	--��ҪĿ���������ʱ����

--����ű�����ս����λ�ƶ�������
COMBAT_UNIT_MOVE_NORMAL = 1		--��ͨ�ƶ�������Ѱ·��
COMBAT_UNIT_MOVE_TELEPORT = 2	--���ͣ�����������λ��
COMBAT_UNIT_MOVE_JUMP = 3		--��Ծ�ƶ�
COMBAT_UNIT_MOVE_RUSH = 4		--����ƶ�

--����ս����λ����״̬������
COMBAT_UNIT_STATUS_NOTMOVE = 1		--��ֹ�ƶ�
COMBAT_UNIT_STATUS_NOTACTION = 2	--��ֹ�ж�
COMBAT_UNIT_STATUS_NOTROUND = 3		--��ֹ�غ�
COMBAT_UNIT_STATUS_IGNOREJOINT = 4	--���Ӻϻ�
COMBAT_UNIT_STATUS_UNBENDING = 5	--����

--���Ա��
FIGHT_ATTR_HP = 3			--����ֵ����
FIGHT_ATTR_MP = 4			--����ֵ����
--DYNAMIC_ATTR_HPMAX = 32		--������������
--DYNAMIC_ATTR_MPMAX = 33		--������������
DYNAMIC_ATTR_HPMAX = 27		--������������
DYNAMIC_ATTR_MPMAX = 28		--������������

--[[����
	1-����
	2-����
	3-����
	4-����
]]--

npcUnits={}
npcUnitsNum = 29
npcUnits[0]=2101001
npcUnits[1]=2101002
npcUnits[2]=2101003
npcUnits[3]=2101005
npcUnits[4]=2101006
npcUnits[5]=2101007
npcUnits[6]=2101012
npcUnits[7]=2101015
npcUnits[8]=2101016
npcUnits[9]=2101017
npcUnits[10]=2101018
npcUnits[11]=2101019
npcUnits[12]=2101020
npcUnits[13]=2101021
npcUnits[14]=2101022
npcUnits[15]=2101024
npcUnits[16]=2101025
npcUnits[17]=2101026
npcUnits[18]=2101027
npcUnits[19]=2101030
npcUnits[20]=2101032
npcUnits[21]=2101033
npcUnits[22]=2101037
npcUnits[23]=2101039
npcUnits[24]=2101045
npcUnits[25]=2101048
npcUnits[26]=2101052
npcUnits[27]=2101053
npcUnits[28]=2101063
npcUnits[29]=2101064

----------------------------------------------------------------------------------------------------------------------------------------
--���鵥λ
----------------------------------------------------------------------------------------------------------------------------------------
--Ĭ�ϵ�ս���ű��������ж�һ��ȫ��
function default_level_script(userID)
    if not check_side_has_unit(userID, COMBAT_CAMP_PASSIVE)
    then
        SET_CAMP_ACTIVE_WIN(userID, 1)--�Է�ȫ��
        return 
    elseif not check_side_has_unit(userID, COMBAT_CAMP_ACTIVE)
    then
        SET_CAMP_ACTIVE_WIN(userID, 0)--����ȫ��
        return
    end
    return
end
--��ȡս����Ӫ�ĵ�λ��
function get_unit_by_side(userID, camp_type)
    local camp = {}
    for i=0,9 do
		camp[i] = GET_COMBAT_SIDE(userID,camp_type,i)
    end
    return camp
end
--��ȡ˫�����е�λ
function get_all_units(userID)
	local units = {}
	units[COMBAT_CAMP_ACTIVE]=get_unit_by_side(userID, COMBAT_CAMP_ACTIVE)
	units[COMBAT_CAMP_PASSIVE]=get_unit_by_side(userID, COMBAT_CAMP_PASSIVE)
	return units
end
--���ս����Ӫ�Ƿ���ս����λ
function check_side_has_unit(userID, camp_type)
    for i=0,9 do
		if GET_COMBAT_SIDE(userID,camp_type,i)>=0 then return true end
    end
    return false
end
--���ս����Ӫ�м������ŵĵ�λ
function get_side_unit_number(userID, camp_type)
	local t = 0
    for i=0,9 do
		if GET_COMBAT_SIDE(userID,camp_type,i)>=0 then t=t+1 end
    end
    return t
end
--���ս����λ
function add_unit(userID, side, monsterid, x, y, direction)
	local newUnitID = -1
	local xx = x
	local yy = y
	if pos_can_stand(userID, x, y) then
		newUnitID = ADD_COMBAT_UNIT(userID, side, monsterid, x, y, direction)
	end
	if newUnitID < 0 and pos_can_stand(userID, x+1, y+0) then
		newUnitID = ADD_COMBAT_UNIT(userID, side, monsterid, x, y, direction)
	end
	if newUnitID < 0 and pos_can_stand(userID, x-1, y+0) then
		newUnitID = ADD_COMBAT_UNIT(userID, side, monsterid, x, y, direction)
	end
	if newUnitID < 0 and pos_can_stand(userID, x+0, y+1) then
		newUnitID = ADD_COMBAT_UNIT(userID, side, monsterid, x, y, direction)
	end
	if newUnitID < 0 and pos_can_stand(userID, x+0, y-1) then
		newUnitID = ADD_COMBAT_UNIT(userID, side, monsterid, x, y, direction)
	end
	if newUnitID < 0 and pos_can_stand(userID, x+1, y-1) then
		newUnitID = ADD_COMBAT_UNIT(userID, side, monsterid, x, y, direction)
	end
	if newUnitID < 0 and pos_can_stand(userID, x+1, y+1) then
		newUnitID = ADD_COMBAT_UNIT(userID, side, monsterid, x, y, direction)
	end
	if newUnitID < 0 and pos_can_stand(userID, x-1, y+1) then
		newUnitID = ADD_COMBAT_UNIT(userID, side, monsterid, x, y, direction)
	end
	if newUnitID < 0 and pos_can_stand(userID, x-1, y-1) then
		newUnitID = ADD_COMBAT_UNIT(userID, side, monsterid, x, y, direction)
	end
	if newUnitID >= 0 then
		adjust_unit_power(userID, newUnitID, 0)	--����ǿ��
		adjust_unit_move(userID, unit_id)		--�����ƶ�
		add_ai_check(userID, newUnitID)			--ע����Ϊ
		return newUnitID
	end
	LOG_INFO("Failed to add unit: %d\n", monsterid)
	return -1
end
--���ƽ��ǿ��
function get_power(userID, units)
	local power=0
	local count=0--�������
	local enemy=0--��������
	for i=1,9 do
		if units[COMBAT_CAMP_ACTIVE][i] >=0 then
			count=count+ 1
			power=power+ GET_COMBAT_UNIT_ATTR(units[COMBAT_CAMP_PASSIVE][0], 6)
			power=power+ GET_COMBAT_UNIT_ATTR(units[COMBAT_CAMP_PASSIVE][0], 7)
			power=power+ GET_COMBAT_UNIT_ATTR(units[COMBAT_CAMP_PASSIVE][0], 8)
			power=power+ GET_COMBAT_UNIT_ATTR(units[COMBAT_CAMP_PASSIVE][0], 9)
			power=power+ GET_COMBAT_UNIT_ATTR(units[COMBAT_CAMP_PASSIVE][0], 10)
			power=power+ GET_COMBAT_UNIT_ATTR(units[COMBAT_CAMP_PASSIVE][0], 11)
		end
		if units[COMBAT_CAMP_PASSIVE][i] >=0 then
			enemy=enemy+1
		end
	end
	power = math.floor(power/count/6)--���ƽ��ǿ��
	return power,count,enemy
end
--����������
function additional_enemy(userID, units, power, count, enemy, backup_x, backup_y)
	local backup_id_low = 2000100 + math.max(1,math.min(power,100)-5)
	local backup_id_high = 2000100 + math.min(power+5,100)
	for i=0,9 do
		if count>enemy then
			if units[COMBAT_CAMP_ACTIVE][i] <0 then
				newUnitID,newUnitIndex = add_unit(userID, units, COMBAT_CAMP_PASSIVE, math.random(backup_id_low,backup_id_high), backup_x, backup_y)
				count=count-1
				--print("enemy added")
			end
		end
	end

end
--
function add_enemy(userID, units, backup_x, backup_y)
	local power=0--���ƽ��ǿ��
	local count=0--�������
	local enemy=0--��������
	power,count,enemy = get_power(userID, units)
	if count>enemy then
		additional_enemy(userID, units, power, count, enemy, backup_x, backup_y)
	end
end
function kill_team(userID, units, side, startIndex)
	for i=startIndex,9 do
		if units[side][i] >=0 then
			KILL_COMBAT_UNIT(userID, units[side][i])--ȥ����Ҳ���������
			units[side][i]=-1
		end
	end
end
--�ض�λ�ò���
function putMonster(userID, x, y, monster_id,direction)
	local tx,ty
	if pos_can_stand(userID, x, y) then
		tx=x
		ty=y
	elseif pos_can_stand(userID, x+1, y+0) then
		tx=x+1
		ty=y+0
	elseif pos_can_stand(userID, x-1, y+0) then
		tx=x-1
		ty=y+0
	elseif pos_can_stand(userID, x+0, y+1) then
		tx=x+0
		ty=y+1
	elseif pos_can_stand(userID, x+0, y-1) then
		tx=x+0
		ty=y-1
	elseif pos_can_stand(userID, x+1, y+1) then
		tx=x+1
		ty=y+1
	elseif pos_can_stand(userID, x-1, y-1) then
		tx=x-1
		ty=y-1
	elseif pos_can_stand(userID, x+1, y-1) then
		tx=x+1
		ty=y-1
	elseif pos_can_stand(userID, x-1, y+1) then
		tx=x-1
		ty=y+1
	else
		return 0
	end
	add_unit(userID, COMBAT_CAMP_PASSIVE, monster_id, tx, ty, direction)
	return 1
end
--����ӹ�
function freemapbattle(userID,level_id,first,monster_id,maxcount)
	local result = 0
	if first==1 then
		unitID = GET_COMBAT_SIDE(userID, COMBAT_CAMP_ACTIVE , 0)	--����
		local x,y = GET_COMBAT_UNIT_POSITION(unitID)				--����λ��
		local count = 0
		if canMove(userID, x, y, 1, 0, 4) then
			count = count + putMonster(userID, x + 3, y, monster_id, 4)
		end
		if canMove(userID, x, y, -1, 0, 4) and count<maxcount then
			count = count + putMonster(userID, x - 3, y, monster_id, 2)
		end
		if canMove(userID, x, y, 0, 1, 4) and count<maxcount then
			count = count + putMonster(userID, x, y + 3, monster_id, 1)
		end
		if canMove(userID, x, y, 0, -1, 4) and count<maxcount then
			count = count + putMonster(userID, x, y - 3, monster_id, 3)
		end
		
		if canMove(userID, x, y, 1, -1, 4) and count<maxcount then
			count = count + putMonster(userID, x + 3, y - 3, monster_id, 3)
		end
		if canMove(userID, x, y, 1, 1, 4) and count<maxcount then
			count = count + putMonster(userID, x + 3, y + 3, monster_id, 3)
		end
		if canMove(userID, x, y, -1, 1, 4) and count<maxcount then
			count = count + putMonster(userID, x - 3, y + 3, monster_id, 3)
		end
		if canMove(userID, x, y, -1, -1, 4) and count<maxcount then
			count = count + putMonster(userID, x - 3, y - 3, monster_id, 3)
		end
	end
	return result
end
--�ض�����������
function canMove(userID, startx, starty, dx, dy, distance)
	for i =0,distance do
		local x = startx + dx*distance
		local y = starty + dy*distance
		if GET_BATTLEFIELD_POS_BLOCK(userID, x, y) > 1 then
			return false
		end
	end
	return true
end
--�ض�λ����Χ�ĵ���
function getEnemyAround(userID, unit_id, x, y)
	local enemy = 0
	local targets = GET_AREA_TARGET_UNIT(userID, x, y, AREA_AROUND, getEnemyCamp(userID, unit_id), 1)
	for key in pairs(targets) do
		enemy = enemy + 1
	end
	return enemy
end

----------------------------------------------------------------------------------------------------------------------------------------
--��λ����
----------------------------------------------------------------------------------------------------------------------------------------
--��ȡ�з���Ӫ����Ҫ����PVP
function getEnemyCamp(userID, unit_id)
	local camp = GET_COMBAT_UNIT_CAMP(userID, unit_id)
	if camp ==1 then
		return 2
	else
		return 1
	end
end
--�ж��Ƿ����
function isEnemy(userID, unit_id, target_id)
	local camp1 = GET_COMBAT_UNIT_CAMP(userID, unit_id)
	local camp2 = GET_COMBAT_UNIT_CAMP(userID, target_id)
	if camp1~=camp2 then
		return true
	else
		return false
	end
end
--���ս����Ӫ�м������ŵ��ض��Ա�Ľ�ɫ
function get_side_gender_number(userID, camp_type, gender_check)
	local t = 0
    local camp = get_unit_by_side(userID, camp_type)
    for i=0,9 do
		if camp[i]>=0 then
			if gender_check == GET_COMBAT_UNIT_EXTRAATTR(userID, camp[i], COMBAT_EXTRAATTR_GENDER) then	t=t+1 end
		end
    end
    return t
end
--���ö���AI
function set_team_ai(userID, units, side, ai, startIndex)
	for i=startIndex,9 do
		if units[side][i] > 0 then
			CHANGE_COMBAT_NPC_AI(userID, units[side][i], ai)
		end
	end
end
--�ӵ�λ��ȡ����ƫ��
function getFacePos(userID, unit_id, distance)
	local face = GET_UNIT_FACE(unit_id)
	return getFaceOff(face, distance)
end
--�������ȡ����ƫ��
function getFaceOff(face, distance)
	local x_off,y_off = 0,0
	if face==1 then y_off = -distance end
	if face==2 then x_off = distance end
	if face==3 then y_off = distance end
	if face==4 then x_off = -distance end
	return x_off, y_off
end
--�������ƫ��
function getDXDY(x, y, target_posx, target_posy)
	local dx,dy
	dx = target_posx-x
	if dx>0 then dx=1 end
	if dx<0 then dx=-1 end
	dy = target_posy-y
	if dy>0 then dy=1 end
	if dy<0 then dy=-1 end
	return dx,dy
end
--�����Ծ����
function getJumpDis(userID, start_posx, start_posy, target_posx, target_posy, step)
	local num = 0
	local x = start_posx
	local y = start_posy
	local x_off = target_posx - start_posx
	local y_off = target_posy - start_posy
	for i=1,step do
		local d = step - i
		local xx = target_posx + d * x_off
		local yy = target_posy + d * y_off
		local block = GET_BATTLEFIELD_POS_BLOCK(userID, xx, yy)
		if block < 3 then	--������
			local target = GET_COMBAT_UNIT_BY_POS(userID, xx, yy)
			if target<0 then	--�޵�λ
				num = d + 1
				x = xx
				y = yy
				break
			end
		end
	end
	return num, x, y
end
--�ж��Ƿ�׹��
function isJumpDown(userID, start_posx, start_posy, target_posx, target_posy, step)
	local n,x,y = getJumpDis(userID, start_posx, start_posy, target_posx, target_posy, step)
	local block = GET_BATTLEFIELD_POS_BLOCK(userID, x, y)
	if block == 2 then
		return true
	else
		return false
	end
end
--��ó�̾���
function getRushDis(userID, caster_unit_id, target_posx, target_posy, step)
	local num = 0
	local x_off,y_off = getFacePos(userID, caster_unit_id, 1)
	for d=0,step-1 do
		local block = GET_BATTLEFIELD_POS_BLOCK(userID, target_posx + d * x_off, target_posy + d * y_off)
		if block < 3 then	--������
			local target = GET_COMBAT_UNIT_BY_POS(userID, target_posx + d * x_off, target_posy + d * y_off)
			if target<0 then
				num = d + 1
			else
				break
			end
		else
			break
		end
	end
	return num
end
--�Ƿ�����
function isMale(user_id, unit_id)
	local gender = GET_COMBAT_UNIT_EXTRAATTR(user_id, unit_id, 1)
	if gender == 101 then
		return true
	else
		return false
	end
end
--�������ڰٷֱ�
function hp_less_than_percent(unit,num)
	local hp = GET_COMBAT_UNIT_ATTR(unit, 3)
	local maxhp = GET_COMBAT_UNIT_ATTR(unit, DYNAMIC_ATTR_HPMAX)
	if hp/maxhp < num/100 then
		return true
	end
	return false
end
--Ŀ���Ƿ�����ض�BUFF
function target_has_buff(unit_id, buff_id)
	local buffs = GET_COMBAT_UNIT_BUFF(unit_id)
	for key in pairs(buffs) do
		if buffs[key] == buff_id then return true end
	end
	return false
end
--��������ض�BUFF�ļ���
function count_team_has_buff(userID, side, buffID)
	local c = 0
	local units = get_unit_by_side(userID, side)
    for i=0,9 do
		if units[i]>=0 then
			if target_has_buff(units[i], buffID) then c = c + 1 end
		end
    end
    return c
end

----------------------------------------------------------------------------------------------------------------------------------------
--�ؿ�����
----------------------------------------------------------------------------------------------------------------------------------------
--ȫ�����
function recover_mp(userID, reg)
	local mp = 0
	local mmp = 0
    for i=0,9 do
		local unit = GET_COMBAT_SIDE(userID,COMBAT_CAMP_ACTIVE,i)
		if unit>=0 then
			mp = GET_COMBAT_UNIT_ATTR(unit, 4)
			mmp = GET_COMBAT_UNIT_ATTR(unit, DYNAMIC_ATTR_MPMAX)
			SET_COMBAT_UNIT_ATTR(userID, unit, 4, mp + mmp*reg)
		end
		unit = GET_COMBAT_SIDE(userID,COMBAT_CAMP_PASSIVE,i)
		if unit>=0 then
			mp = GET_COMBAT_UNIT_ATTR(unit, 4)
			mmp = GET_COMBAT_UNIT_ATTR(unit, DYNAMIC_ATTR_MPMAX)
			SET_COMBAT_UNIT_ATTR(userID, unit, 4, mp + mmp*reg)
		end
    end
end
--ȫ���Ѫ
function recover_hp(userID, reg)
	local hp = 0
	local mhp = 0
    for i=0,9 do
		local unit = GET_COMBAT_SIDE(userID,COMBAT_CAMP_ACTIVE,i)
		if unit>=0 then
			hp = GET_COMBAT_UNIT_ATTR(unit, 3)
			mhp = GET_COMBAT_UNIT_ATTR(unit, DYNAMIC_ATTR_HPMAX)
			SET_COMBAT_UNIT_ATTR(userID, unit, 3, hp + mhp*reg)
		end
		unit = GET_COMBAT_SIDE(userID,COMBAT_CAMP_PASSIVE,i)
		if unit>=0 then
			hp = GET_COMBAT_UNIT_ATTR(unit, 3)
			mhp = GET_COMBAT_UNIT_ATTR(unit, DYNAMIC_ATTR_HPMAX)
			SET_COMBAT_UNIT_ATTR(userID, unit, 3, hp + mhp*reg)
		end
    end
end
--����ǿ�ȵ���
function team_adjust(userID, side, reg)
	local hp = 0
	local mhp = 0
    for i=0,9 do
		local unit = GET_COMBAT_SIDE(userID,side,i)
		if unit>=0 then
			--maxhp,hp
			local mhp = GET_COMBAT_UNIT_ATTR(unit, DYNAMIC_ATTR_HPMAX)
			SET_COMBAT_UNIT_ATTR(userID, unit, DYNAMIC_ATTR_HPMAX, mhp * reg)
			SET_COMBAT_UNIT_ATTR(userID, unit, 3, 1+mhp * reg)
			--�˺�
			local dmg = GET_COMBAT_UNIT_ATTR(unit, 20)
			SET_COMBAT_UNIT_ATTR(userID, unit, 20, 1+dmg * reg)
		end
    end
end

--ͨ�õ��ֵ��ж�ִ��BUFF�����ڵ���AI�����ž���ȣ�
function add_ai_check_all(userID)
	local unit
    for i=0,9 do
		unit = GET_COMBAT_SIDE(userID,COMBAT_CAMP_ACTIVE,i)
		if unit>=0 then
			add_ai_check(userID,unit)
		end
		unit = GET_COMBAT_SIDE(userID,COMBAT_CAMP_PASSIVE,i)
		if unit>=0 then
			add_ai_check(userID,unit)
		end
    end
end
function add_ai_check(userID, unit_id)
	ADD_UNIT_BUFF(userID, unit_id, unit_id, 1000)
end

--���5����λ����PVPս��
function set5RandomUnit(userID, units)
	local u={}
	u[0]=2101001
	u[1]=2101002
	u[2]=2101003
	u[3]=2101005
	u[4]=2101006
	u[5]=2101007
	u[6]=2101012
	u[7]=2101015
	u[8]=2101016
	u[9]=2101017
	u[10]=2101018
	u[11]=2101019
	u[12]=2101020
	u[13]=2101021
	u[14]=2101022
	u[15]=2101024
	u[16]=2101025
	u[17]=2101026
	u[18]=2101027
	u[19]=2101030
	u[20]=2101032
	u[21]=2101033
	u[22]=2101037
	u[23]=2101039
	u[24]=2101045
	u[25]=2101048
	u[26]=2101052
	u[27]=2101053
	u[28]=2101063
	u[29]=2101064
		
	add_unit(userID, units, COMBAT_CAMP_PASSIVE, u[math.random(0,29)], 16, 14, 4)
	add_unit(userID, units, COMBAT_CAMP_PASSIVE, u[math.random(0,29)], 16, 15, 4)
	add_unit(userID, units, COMBAT_CAMP_PASSIVE, u[math.random(0,29)], 14, 15, 4)
	add_unit(userID, units, COMBAT_CAMP_PASSIVE, u[math.random(0,29)], 14, 16, 4)
	add_unit(userID, units, COMBAT_CAMP_PASSIVE, u[math.random(0,29)], 12, 16, 4)
end
START_TIME = 0
TP2HH = 4 * 10
TP2DD = 6 * TP2HH
TP2MM = 3 * TP2DD
TP2YY = 12 * TP2MM
--��ȡ��Ϸ��ǰ������ʱ
function get_role_date(userID)
	local timePoint = GET_ROLE_TIME(userID)
	local y = timePoint / TP2YY
	local YY = math.floor(y)
	local m = (y - YY) / TP2MM
	local MM = 1 + math.floor(m)
	local d = (m - MM) / TP2DD
	local DD = 1 + math.floor(d)
	local t = (d - DD) / TP2HH
	local TT = 1 + math.floor(t)
	return YY,MM,DD,TT
end
--��ɫǿ����ʱ��仯����
local hero_power = {}
function init_hero_power()
	hero_power[2101010]=160
	hero_power[2101023]=132
	hero_power[2101054]=128
	hero_power[2101034]=126
	hero_power[2101055]=126
	hero_power[2101052]=120
	hero_power[2101012]=120
	hero_power[2200023]=116
	hero_power[2101044]=116
	hero_power[2101046]=114
	hero_power[2101021]=112
	hero_power[2101017]=112
	hero_power[2101011]=108
	hero_power[2101009]=106
	hero_power[2101064]=106
	hero_power[2101006]=106
	hero_power[2101030]=106
	hero_power[2101016]=106
	hero_power[2101001]=104
	hero_power[2101043]=104
	hero_power[2300123]=104
	hero_power[2101056]=104
	hero_power[2101038]=104
	hero_power[2300527]=104 --��ϼ��
	hero_power[2101018]=102
	hero_power[2101019]=102
	hero_power[2200016]=102
	hero_power[2101061]=100
	hero_power[2101041]=100
	hero_power[2101060]=100
	hero_power[2101048]=98
	hero_power[2101014]=98
	hero_power[2101033]=98
	--hero_power[2101035]=98
	hero_power[2300166]=98 --������
	hero_power[2101068]=96
	hero_power[2101015]=96
	hero_power[2101057]=96
	hero_power[2101045]=96
	hero_power[2101058]=96
	hero_power[2101036]=96
	hero_power[2101008]=94
	hero_power[2101005]=94
	hero_power[2101025]=94
	hero_power[2101028]=94
	hero_power[2101029]=94
	hero_power[2200014]=94
	hero_power[2101020]=92
	hero_power[2101024]=92
	hero_power[2101037]=92
	hero_power[2101059]=90
	hero_power[2101002]=90
	hero_power[2101063]=90
	hero_power[2101069]=90
	hero_power[2101039]=88
	hero_power[2101031]=88
	hero_power[2200015]=88
	hero_power[2101026]=84
	hero_power[2101003]=80
	hero_power[2101053]=78
	hero_power[2200013]=76
	hero_power[2200010]=76
	hero_power[2200012]=72
	hero_power[2101042]=70
	hero_power[2101062]=70
	hero_power[2101027]=70
	hero_power[2101049]=68
	hero_power[2101022]=68
	hero_power[2101040]=66
	hero_power[2101032]=66
	hero_power[2101007]=64
	hero_power[2101004]=64
	hero_power[2200011]=62
	hero_power[2101050]=60
	hero_power[2101051]=60
end
init_hero_power()
--��ȡ��ɫ����ǿ��
local max_power = 200
local min_power = 60
local max_percent = 90
local min_percent = 10
local good_exp = 2.5
local good_a = (max_percent-min_percent)/(max_power^good_exp-min_power^good_exp)
local good_b = max_percent-max_power^good_exp*good_a
local evil_exp = 1.5
local evil_a = (max_percent-min_percent)/(max_power^evil_exp-min_power^evil_exp)
local evil_b = max_percent-max_power^evil_exp*evil_a
function getBasePower(unit_power, good_or_evil)
	local p = unit_power
	if good_or_evil > 0 then
		p = unit_power^good_exp*good_a+good_b
	else
		p = unit_power^evil_exp*evil_a+evil_b
	end
	return p
end
--����Ϸʱ�������λǿ��
function adjust_unit_power(userID, unit_id, time_offset)
    local maxpower = hero_power[GET_COMBAT_UNIT_EXTRAATTR(userID, unit_id, 3)]
    if maxpower then
		local basepower = getBasePower(maxpower, GET_COMBAT_UNIT_EXTRAATTR(userID, unit_id, 2))
		--��ȡ���Էֲ�
		local attr_1 = GET_COMBAT_UNIT_ATTR(unit_id, 6)/basepower
		local attr_2 = GET_COMBAT_UNIT_ATTR(unit_id, 7)/basepower
		local attr_3 = GET_COMBAT_UNIT_ATTR(unit_id, 8)/basepower
		local attr_4 = GET_COMBAT_UNIT_ATTR(unit_id, 9)/basepower
		local attr_5 = GET_COMBAT_UNIT_ATTR(unit_id, 10)/basepower
		local attr_6 = GET_COMBAT_UNIT_ATTR(unit_id, 11)/basepower
		local time_pass = GET_ROLE_TIME(userID)
		local train_time = 5 * TP2YY --�������5����Դӻ���ֵ����������ֵ
		local currentpower = basepower+ math.floor((maxpower - basepower) * (time_pass+time_offset) / train_time)
		
		--����һ������
		attr_1 = attr_1 * currentpower
		attr_2 = attr_2 * currentpower
		attr_3 = attr_3 * currentpower
		attr_4 = attr_4 * currentpower
		attr_5 = attr_5 * currentpower
		attr_6 = attr_6 * currentpower
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 6, attr_1)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 7, attr_2)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 8, attr_3)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 9, attr_4)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 10, attr_5)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 11, attr_6)
		
		--������������
		--maxhp,hp
		SET_COMBAT_UNIT_ATTR(userID, unit_id, DYNAMIC_ATTR_HPMAX, attr_1*30)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 3, attr_1*30)
		--maxmp,mp
		SET_COMBAT_UNIT_ATTR(userID, unit_id, DYNAMIC_ATTR_MPMAX, attr_3*20)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 4, attr_3*20)
		--����,����
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 12, attr_1*6)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 13, attr_1*6)
		--����,����
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 14, attr_4*6)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 15, attr_4*6)
		--����,����
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 16, attr_2*6)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 17, attr_2*6)
		--��׼,��
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 18, attr_3*6)
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 19, attr_3*6)
		--�˺�
		SET_COMBAT_UNIT_ATTR(userID, unit_id, 20, attr_1 + attr_2 + attr_3 + attr_4 + attr_5 + attr_6)
		
		--local t = GET_COMBAT_UNIT_ATTR(unit_id, 12)
		--checkValue(userID, unit_id, -t)
	end
end
--�������еĵ�λ���ƶ�
function adjust_units_move(userID)
	local unit_id = -1
    for i=0,9 do
		unit_id = GET_COMBAT_SIDE(userID, COMBAT_CAMP_ACTIVE, i)
		if unit_id>=0 then	adjust_unit_move(userID, unit_id) end
		unit_id = GET_COMBAT_SIDE(userID, COMBAT_CAMP_PASSIVE, i)
		if unit_id>=0 then	adjust_unit_move(userID, unit_id) end
    end
end
--�����Ṧֵ������λ�ƶ�
function adjust_unit_move(userID, unit_id)
	local qinggong = GET_COMBAT_UNIT_ATTR(unit_id, 10)
	local move_add = 0
	if qinggong >= 30 then move_add = 10 end
	if qinggong >= 60 then move_add = 20 end
	if qinggong >= 90 then move_add = 30 end
	local basemove = GET_COMBAT_UNIT_ATTR(unit_id, 5)
	SET_COMBAT_UNIT_ATTR(userID, unit_id, 5, basemove + move_add)
end
function adjust_monster_power(userID, startpower, endpower, years)
	--ǿ�Ȼ��
	local time_pass = GET_ROLE_TIME(userID)
	local attr_1 = startpower + (endpower-startpower) * time_pass/(TP2YY * years)
	local attr_2 = startpower + (endpower-startpower) * time_pass/(TP2YY * years)
	local attr_3 = startpower + (endpower-startpower) * time_pass/(TP2YY * years)
	local attr_4 = startpower + (endpower-startpower) * time_pass/(TP2YY * years)
	local attr_5 = startpower + (endpower-startpower) * time_pass/(TP2YY * years)
	local attr_6 = startpower + (endpower-startpower) * time_pass/(TP2YY * years)
	--��������
	enemy = get_unit_by_side(userID, COMBAT_CAMP_PASSIVE)
	for i=0,9 do
		unit_id = enemy[i]
		if unit_id>=0 then
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 6, attr_1-10 + math.random(0,20))
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 7, attr_2-10 + math.random(0,20))
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 8, attr_3-10 + math.random(0,20))
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 9, attr_4-10 + math.random(0,20))
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 10, attr_5-10 + math.random(0,20))
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 11, attr_6-10 + math.random(0,20))
			--maxhp,hp
			SET_COMBAT_UNIT_ATTR(userID, unit_id, DYNAMIC_ATTR_HPMAX, attr_1*30)
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 3, attr_1*30)
			--maxmp,mp
			SET_COMBAT_UNIT_ATTR(userID, unit_id, DYNAMIC_ATTR_MPMAX, attr_3*20)
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 4, attr_3*20)
			--����,����
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 12, attr_1*6)
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 13, attr_1*6)
			--����,����
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 14, attr_4*6)
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 15, attr_4*6)
			--����,����
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 16, attr_2*6)
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 17, attr_2*6)
			--��׼,��
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 18, attr_3*6)
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 19, attr_3*6)
			--�˺�
			SET_COMBAT_UNIT_ATTR(userID, unit_id, 20, attr_1 + attr_2 + attr_3 + attr_4 + attr_5 + attr_6)
		end
	end
end
--����Ϸʱ�������λǿ�ȣ�ȫ�����)
function adjust_enemy_power(userID)
    for i=0,9 do
		local unit_id = GET_COMBAT_SIDE(userID, COMBAT_CAMP_PASSIVE, i)
		if unit_id>=0 then	adjust_unit_power(userID, unit_id, 0) end
    end
end
--λ���Ƿ����
function pos_can_stand(userID, x, y)
	if GET_BATTLEFIELD_POS_BLOCK(userID, x, y) == 1 then
		if GET_COMBAT_UNIT_BY_POS(userID, x, y) < 0 then
			return true
		end
	end
	return false
end
--�ض���λ�Ƿ�����
function heroDead(userID, heroID, campside)
    for i=0,9 do
		local unitID = GET_COMBAT_SIDE(userID,campside,i)
		if unitID >= 0 then
			if GET_COMBAT_UNIT_EXTRAATTR(userID, unitID, 3) == heroID then
				return false
			end
		end
    end
	return true
end
----------------------------------------------------------------------------------------------------------------------------------------
--��������
----------------------------------------------------------------------------------------------------------------------------------------
--����Ч��
bad_buffs = {}
function init_bad_buffs()
	for i=1,10 do
		--��������1
		table.insert(bad_buffs, 11100+i )
		table.insert(bad_buffs, 21100+i )
		table.insert(bad_buffs, 31100+i )
		table.insert(bad_buffs, 41100+i )
		table.insert(bad_buffs, 51100+i )
		table.insert(bad_buffs, 61100+i )
		--��������2
		table.insert(bad_buffs, 111100+i )
		table.insert(bad_buffs, 121100+i )
		table.insert(bad_buffs, 131100+i )
		table.insert(bad_buffs, 141100+i )
		table.insert(bad_buffs, 151100+i )
		table.insert(bad_buffs, 161100+i )
		table.insert(bad_buffs, 171100+i )
		table.insert(bad_buffs, 181100+i )
		table.insert(bad_buffs, 191100+i )
	end
	--С������⼼��
	table.insert(bad_buffs, 10020201 ) --��������
	table.insert(bad_buffs, 10030000 ) --��е
	table.insert(bad_buffs, 10040201 ) --�������
	table.insert(bad_buffs, 10050201 ) --���ﵶ��
	table.insert(bad_buffs, 10310101 ) --��е
	table.insert(bad_buffs, 10310201 ) --��е
	table.insert(bad_buffs, 10330301 ) --�Ʒ�����
	table.insert(bad_buffs, 10520201 ) --������Ȼ
	table.insert(bad_buffs, 10630201 ) --��������
end
init_bad_buffs()
function is_bad_buff(buff_id)
	for key in pairs(bad_buffs) do
		if buff_id == bad_buffs[key] then
			return true
		end
	end
	return false
end
function has_bad_buff(unit_id)
	local buffs = GET_COMBAT_UNIT_BUFF(unit_id)
	for key in pairs(buffs) do
		if is_bad_buff(buffs[key]) then return true end
	end
	return false
end
function clear_bad(userID, unit_id)
	for key in pairs(bad_buffs) do
		DEL_COMBAT_UNIT_BUFF(userID, unit_id, bad_buffs[key])
	end
end
--���Ƽ���
control_buffs = {}
function init_control_buffs()
	for i=1,5 do
		table.insert(control_buffs, 1000+i ) --�ж�
		table.insert(control_buffs, 1100+i ) --�ƶ�
		table.insert(control_buffs, 1200+i ) --�չ�
		table.insert(control_buffs, 1300+i ) --��ʽ
		table.insert(control_buffs, 1400+i ) --����
	end
	table.insert(control_buffs, 10150401)	--ʫ�黭��
	table.insert(control_buffs, 10160201)	--�仨����
	table.insert(control_buffs, 10170401)	--���λ�λ
	table.insert(control_buffs, 10200201)	--���ȷ�Ѩ
	table.insert(control_buffs, 10550302)	--�Ȼ�ħ��
	table.insert(control_buffs, 10550303)	--�Ȼ�ħ��
	table.insert(control_buffs, 10550401)	--��������
end
init_control_buffs()
function is_control_buff(buff_id)
	for key in pairs(control_buffs) do
		if buff_id == control_buffs[key] then
			return true
		end
	end
	return false
end
function has_control_buff(unit_id)
	local buffs = GET_COMBAT_UNIT_BUFF(unit_id)
	for key in pairs(buffs) do
		if is_control_buff(buffs[key]) then return true end
	end
	return false
end
function clear_control(userID, unit_id)
	for key in pairs(control_buffs) do
		DEL_COMBAT_UNIT_BUFF(userID, unit_id, control_buffs[key])
	end
end

dmgUP_buffs = {}
function init_dmgUP_buffs()
	for i=1,10 do
		--��������2
		table.insert(dmgUP_buffs, 190100+i )
		table.insert(dmgUP_buffs, 10120401+i )	--ħ������
	end
	--С������⼼��
	table.insert(dmgUP_buffs, 10680301)	--��Ѫ����
	table.insert(dmgUP_buffs, 10600201)	--ǧ������
	table.insert(dmgUP_buffs, 10600202)	--ǧ������
	table.insert(dmgUP_buffs, 10600203)	--ǧ������
	table.insert(dmgUP_buffs, 10600204)	--ǧ������
	table.insert(dmgUP_buffs, 10600205)	--ǧ������
	table.insert(dmgUP_buffs, 10600206)	--ǧ������
end
init_dmgUP_buffs()
function is_dmgUP_buff(buff_id)
	for key in pairs(dmgUP_buffs) do
		if buff_id == dmgUP_buffs[key] then
			return true
		end
	end
	return false
end
function has_dmgUP_buff(unit_id)
	local buffs = GET_COMBAT_UNIT_BUFF(unit_id)
	for key in pairs(buffs) do
		if is_dmgUP_buff(buffs[key]) then return true end
	end
	return false
end

--�����˺�����
function clear_poison(userID, unit_id)
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10390101 )	--��Ч1~5��
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10390102 )
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10390103 )
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10390104 )
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10390105 )
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10530000 )	--���Ʊ��
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10530201 )	--Ǳ����Ӱ
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10180301 ) --��������
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10190201 )	--��а��
	DEL_COMBAT_UNIT_BUFF(userID, unit_id,10190301 )	--�߲�׷��
end

----------------------------------------------------------------------------------------------------------------------------------------
--����
----------------------------------------------------------------------------------------------------------------------------------------
--����ض�ֵ�������ڲ��ԣ�
function checkValue(userID, unit_id, value)
	local hp = GET_COMBAT_UNIT_ATTR(unit_id, 3)
	SET_COMBAT_UNIT_ATTR(userID, unit_id, 3, hp+value) --ͨ����Ѫ�����ֵ
end
--ս����������
function getTrapNum(userID)
	local trap_num = 0
	local traps = GET_COMBAT_TRAP_INFO(userID)
	for key in pairs(traps) do
		trap_num = trap_num +1
	end
	return trap_num
end
--�Ƿ�PVP
function isPVP(unit_a, unit_b)
	local ai1 = GET_COMBAT_UNIT_AI(unit_a)
	local ai2 = GET_COMBAT_UNIT_AI(unit_b)
	if (ai1+ai2)==0 then
		return true
	else
		return false
	end
end
--�չ���Χ����
function get_normal_attack_range(user_id, unit_id)
	local weapon = GET_COMBAT_UNIT_WEAPON_TYPE(user_id, unit_id)
	if weapon == 0 then return 204 end	--ƽȭ
	if weapon == 1 then return 101 end	--����
	if weapon == 2 then return 102 end	--����
	if weapon == 3 then return 103 end	--����
	if weapon == 4 then return 104 end	--ȭ��
	if weapon == 5 then return 105 end	--����
	if weapon == 6 then return 106 end	--����
end
--��ӱ��
function add_mark(userID,unit_id, added)
	local t = GET_COMBAT_UNIT_ATTR(unit_id, 25)
	SET_COMBAT_UNIT_ATTR(userID, unit_id, 25, t + added) --�԰���ֵ����Ϊ���
end
--��ñ��
function get_mark(unit_id)
	return GET_COMBAT_UNIT_ATTR(unit_id, 25)
end
--��ñ��
function set_mark(userID, unit_id, value)
	SET_COMBAT_UNIT_ATTR(userID, unit_id, 25, value)
end
--�˳�ս��
function quitBattle(userID)
	for i=0,9 do
		local unit = GET_COMBAT_SIDE(userID,COMBAT_CAMP_ACTIVE,i)
		if unit>=0 then
			SET_COMBAT_UNIT_ATTR(userID, unit, 3, 0)
		end
	end
end



























