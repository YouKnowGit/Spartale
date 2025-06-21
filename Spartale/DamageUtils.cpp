#include "DamageUtils.h"
#include "Actor.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"

#include <random>

namespace DamageUtils
{
    float CalculateDamage(const Actor* SourceActor, const Actor* TargetActor, float ADRatio, float APRatio, EDamageType DamageType)
    {
        if (!SourceActor || !TargetActor) return 0.0f;

        const AttributeSet* SourceStats = SourceActor->GetAbilityComponent()->GetAttributeSet();
        const AttributeSet* TargetStats = TargetActor->GetAbilityComponent()->GetAttributeSet();

        if (!SourceStats || !TargetStats) return 0.0f;

        float Damage = 0.0f;

        // ������ Ÿ�Կ� ���� ��� ����� �б�
        switch (DamageType)
        {
        case EDamageType::Physical:
            // ���� ������ = (�� * ���� ���) - (����� ����)
            Damage = SourceStats->Strength.CurrentValue * ADRatio;
            Damage -= TargetStats->Defence.CurrentValue;
            break;

        case EDamageType::Magical:
            // ���� ������ = (���� * ���� ���) - (����� ���� ���׷�)
            Damage = SourceStats->Intelligence.CurrentValue * APRatio;
            Damage -= TargetStats->MagicResistance.CurrentValue;
            break;

        case EDamageType::True:
            // ���� ������ = (�� * ���� ���) �Ǵ� (���� * ���� ���). ���/���� ���� (���� ���� X)
            break;

        case EDamageType::None:
        default:
            break;
        }

        // �ּ� �������� 1�� ����
        if (Damage < 1.f)
        {
            Damage = 1.f;
        } 
        
        {
            // ������ ������ ���� ���� �߻� ����
            // static���� �����Ͽ�, �� �Լ��� ó�� ȣ��� �� �� �� ���� ����
            static std::random_device rd;  // ���� �Ұ����� �õ尪�� ����
            static std::mt19937 gen(rd()); // �õ尪���� �޸��� Ʈ������ ������ �ʱ�ȭ

            // 0.95f ~ 1.15f ������ �Ǽ��� �յ��ϰ� �����ϴ� ������ ���� (-15% ~ +15% ����)
            static std::uniform_real_distribution<float> distrib(0.95f, 1.15f);

            // ���� �������� ���� ������ ���մϴ�.
            Damage *= distrib(gen);
        }

        return Damage;
    }
}