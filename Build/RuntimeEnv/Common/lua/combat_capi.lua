--  C++�ṩ��lua���õĽӿ�,ֻ�����ʹ�ã�������

--��ȡս����Ӫս����λunitid,-1��ʾ�Ƿ�
function GET_COMBAT_SIDE(uin, camp, unit_index)
	return -1
end

--��ȡս����λ��������Ϣ
function GET_COMBAT_UNIT_ATTR(unit_id, attr_type)
	print('GET_COMBAT_UNIT_ATTR')
	return 100
end
--[[
	DYNAMIC_ATTR_XIANGONG = 30, // �ȹ�ֵ����
    DYNAMIC_ATTR_QINGGONG = 31, // �Ṧ��������
    DYNAMIC_ATTR_HPMAX = 32, // ������������
    DYNAMIC_ATTR_MPMAX = 33, // ������������
]]--

--��ȡս����λ����Ӫ��Ϣ
function GET_COMBAT_UNIT_CAMP(uin, unit_id)
	print('GET_COMBAT_UNIT_CAMP')
	return 1
end

--����ս��ʤ��
function SET_CAMP_ACTIVE_WIN(uin, resultType)
	print('SET_CAMP_ACTIVE_WIN:' .. resultType)
	return 1
end

--��ȡս���غ���
function GET_COMBAT_ROUND_NUM(uin)
	--print('GET_COMBAT_ROUND_NUM')
	battleTurn = battleTurn +1
	return battleTurn
end

--�ж�ս���غ��Ƿ����
function IS_COMBAT_ROUND_END(uin)
	--print('IS_COMBAT_ROUND_END')
	return true
end

--���ս����λ, ����<0ʧ�ܣ�>=0��ʾunitID
function ADD_COMBAT_UNIT(uin, camp, config_id, posx, posy, direction)
	print('ADD_COMBAT_UNIT:' .. config_id)
	return 0
end

--��ȡս����λ��AI
function GET_COMBAT_UNIT_AI(unit_id)
	print('GET_COMBAT_UNIT_AI')
end

--�л�ս��NPC��λ��AI
function CHANGE_COMBAT_NPC_AI(uin, unit_id, newAI)
	print('CHANGE_COMBAT_NPC_AI')
end

--��Ӷ����ս������
--is_immediate: 1��ʾ�������ӽ���; 0��ʾս�����������ӽ���
function ADD_EXTRA_COMBAT_DROPID(uin, drop_id, is_immediate)
	print('ADD_EXTRA_COMBAT_DROPID')
end

--ǿ��ɱ��ս����λ
function KILL_COMBAT_UNIT(uin, unit_id)
	print('KILL_COMBAT_UNIT')
end

--��ӡ������־��Ϣ
function LOG_INFO(...)
	print(string.format(...))
end

--ս�������ӻ���
function ADD_COMBAT_TRAP(uin, camp, trap_id, posx, posy, face)
    print('ADD_COMBAT_TRAP')
end

--ɾ��ս���ϵĻ���
function DELETE_COMBAT_TRAP(uin, trap_obj_id)
    print('DELETE_COMBAT_TRAP')
end

--����ս���ϵĻ���
function TRIGGER_COMBAT_TRAP(uin, trap_obj_id)
    print('TRIGGER_COMBAT_TRAP')
end

--��ս����λ���buff
function ADD_UNIT_BUFF(uin, cast_unit_id, target_unit_id, buff_id)
    print('ADD_UNIT_BUFF')
end

--ȡս�������л��ص�{config_id=trap_obj_id, ...}
function GET_COMBAT_TRAP_INFO(uin)
    print('GET_COMBAT_TRAP_INFO')
    return ({{1,2},})
end

--��ȡս����λ������С��0��ʾ����
function GET_UNIT_FACE(unit_id)
    print('GET_UNIT_FACE')
    return 2
end

--��ȡս����λ�����ķ���,С��0��ʾ����
function GET_ATTACK_FACE(cast_unit_id, target_unit_id)
    print('GET_ATTACK_FACE')
    return 2
end

--��ȡս����λ�ĵ�ǰ����x,y, x<0 ��ʾ�Ƿ���λ��
function GET_COMBAT_UNIT_POSITION(unit_id)
    print('GET_COMBAT_UNIT_POSITION')
    return 1,2
end

--��ȡս����λ�������ԣ����Ͷ�����combat_common.lua��,���ػ�ȡ��ֵ
--COMBAT_EXTRAATTR_GENDER = 1	--ս����λ���Ա�
--COMBAT_EXTRAATTR_GOOD = 2		--�ƶ�ֵ
--COMBAT_EXTRAATTR_CONFIGID = 3	--ս����λ���õ�ID
function GET_COMBAT_UNIT_EXTRAATTR(uin, unit_id, extra_attr_type)
    print('GET_COMBAT_UNIT_EXTRAATTR')
    return 1
end

--��ȡս����λ���ϵ�װ��ID��û�з���0
function GET_COMBAT_UNIT_EQUIP(uin, unit_id, equip_type)
    print('GET_COMBAT_UNIT_EXTRAATTR')
    return 0
end

--��ȡս����λ���������ͣ�û�з���0
function GET_COMBAT_UNIT_WEAPON_TYPE(unit_id)
    print('GET_COMBAT_UNIT_WEAPON_TYPE')
    return 0
end

--����ս����λ��������ֵ
function SET_COMBAT_UNIT_ATTR(uin, unit_id, attr_type, attr_value)
    print('SET_COMBAT_UNIT_ATTR')
end

--��ȡս����λ���ϵ�BUFF����ID�б�, <0��ʾ�Ƿ�
function GET_COMBAT_UNIT_BUFF(unit_id)
    print('GET_COMBAT_UNIT_BUFF')
    return {1,2,3,4,5}
end

--ɾ��ս����λ���ϵ�BUFF
function DEL_COMBAT_UNIT_BUFF(uin, unit_id, buff_id)
    print('DEL_COMBAT_UNIT_BUFF')
end

--��ȡ��Ŀ�굥λ�Ĺ������� 1-7�� ����0��ʾ��������
function GET_TARGET_ATTACK_DISTANCE(cast_unit_id, target_unit_id, target_range_id)
    print('GET_TARGET_ATTACK_DISTANCE')
    return 3
end

--��ȡս����ǰ��״̬,status_type Ϊ0-7������0��1
function GET_COMBAT_STATUS(uin, status_type)
    print('GET_COMBAT_STATUS')
    return 1
end

--����ս����ǰ��״̬, status_type Ϊ0-7, is_status_setΪ0 1,��ʾ�������״̬
function SET_COMBAT_STATUS(uin, status_type, is_status_set)
    print('SET_COMBAT_STATUS')
end

--�ű�ʹ��ս������
function UNIT_CAST_SKILL(uin, cast_unit_id, target_pos_x, target_pos_y, skill_id)
    print('UNIT_CAST_SKILL')
end

--��ս����λ��������λ��,���area_id==0��ʾֱ�Ӵ��ͣ�move_type��ʾλ������
--move_type������: 
--  COMBAT_UNIT_MOVE_NORMAL = 1		--��ͨ�ƶ�������Ѱ·��
--  COMBAT_UNIT_MOVE_TELEPORT = 2	--���ͣ�����������λ��	û��·���Ĺ��̣�˲�䵽��
--  COMBAT_UNIT_MOVE_JUMP = 3		--��Ծ�ƶ�				λ�ƹ����ж�����Լ��λ�ƶ�����ţ���FPS15_0����
--  COMBAT_UNIT_MOVE_RUSH = 4		--����ƶ�				λ�ƺ󲥷Ŷ�����Լ��λ�ƶ�����ţ���FPS15_1����
function JUMP_COMBAT_UNIT(uin, unit_id, area_id, target_posx, target_posy, move_type, anim_type)
    print('JUMP_COMBAT_UNIT')
end

--����ս��λ�û�ȡս����λunit_id, <0 ��ʾû��
function GET_COMBAT_UNIT_BY_POS(uin, pos_x, pos_y)
    print('GET_COMBAT_UNIT_BY_POS')
    return 1
end

--����ս��ս����λ�Ļ���ģʽ
function SET_COMBAT_UNIT_MOTORMODE(unit_id, target_posx, target_posy, distance, ai_id)
    print('SET_COMBAT_UNIT_MOTORMODE')
end

--����ս����λ���ϵ�״̬
--status_type �����ͣ�
--  COMBAT_UNIT_STATUS_NOTMOVE = 1,		��ֹ�ƶ�
--  COMBAT_UNIT_STATUS_NOTACTION = 2,	��ֹ�ж�
--	COMBAT_UNIT_STATUS_NOTROUND = 3,	��ֹ�غ�
--  COMBAT_UNIT_STATUS_IGNOREJOINT = 4,	���Ӻϻ�
--  COMBAT_UNIT_STATUS_UNBENDING = 5	����
--is_set��
--	0 ��ʾ���״̬
--	1 ��ʾ����״̬
function SET_COMBAT_UNIT_STATUS(unit_id, status_type, is_set)
	print('SET_COMBAT_UNIT_STATUS')
end

--��ȡunit_id��λ��󹥻���ս����λID
function GET_LAST_ATTACK_UNIT(unit_id)
    print('GET_LAST_ATTACK_UNIT')
    return -1
end

--��ȡָ��λ�÷�Χ��ĳ��Ӫ��λ���б�
function GET_AREA_TARGET_UNIT(uin, pos_x, pos_y, area_id, camp, direction)
    print('GET_AREA_TARGET_UNIT')
    return {}
end

--��ȡս����λ��Score��ֵ
function GET_UNIT_SCORE(unit_id, score_id)
    print('GET_UNIT_SCORE')
    return 0
end

function MOVIE_ACTION(userID, a,b,c)
    print('MOVIE_ACTION')
end

function GET_ROLE_TIME(userID)
    print('GET_ROLE_TIME')
    return 4
end

--��ȡ����ʹ�ü��ܵĵȼ�,����0��ʾ��ȡʧ��
function GET_ROLEUNIT_SKILL_LEVEL(uin, unit_id, skill_id)
    print('GET_ROLEUNIT_SKILL_LEVEL')
    return 1
end

--��ȡս������赲����
--����ֵ����: 1 ������; 2 �����赲������; 3 ��������
function GET_BATTLEFIELD_POS_BLOCK(uin, pos_x, pos_y)
    print('GET_BATTLEFIELD_POS_BLOCK')
    return 1
end




