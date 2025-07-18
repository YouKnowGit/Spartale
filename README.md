# Desperado

**프로젝트 제작 기간:** 2024.06.20 ~ 2024.06.26

## ⚔️ 프로젝트 소개

### 프롤로그
기나긴 전쟁의 끝, 승리를 눈앞에 둔 순간.
하늘이 갈라지며 강림한 고대의 재앙, 검은 용 **'데스페라도'**는 모든 것을 앗아갔다.

소중한 동료 '라일라'를 잃고, 절망의 잿더미 속에서 홀로 살아남은 당신.
복수와 정의, 그 경계에 선 여정이 지금 시작된다.

### Desperado란?
`Desperado`는 C++과 Windows 콘솔 환경을 기반으로 제작된 클래식 턴제 텍스트 RPG입니다. 플레이어는 동료의 복수와 왕국의 운명을 짊어지고 절차적으로 생성된 미궁을 탐험하며, 강력한 몬스터들과의 전략적인 턴제 전투를 통해 성장하여 모든 것의 원흉인 '데스페라도'를 물리쳐야 합니다.

[![Watch the video](https://img.youtube.com/vi/vGe3DBCeu0Y/0.jpg)](https://www.youtube.com/watch?v=vGe3DBCeu0Y)  
🔗 [**Desperado 시연영상 전체 보기 ▶**](https://www.youtube.com/watch?v=vGe3DBCeu0Y)

게임은 필드 탐험과 턴제 전투, 두 가지 핵심 파트로 진행됩니다.
-   **필드 탐험:** 플레이어는 마을에서 정비를 마친 뒤, 포탈을 통해 다양한 테마의 필드로 이동합니다. 필드를 자유롭게 이동하며 NPC와 상호작용하고, 위험지대에서는 무작위로 몬스터와 조우하게 됩니다.
-   **턴제 전투:** 몬스터와 조우하면 별도의 전투 화면으로 전환됩니다. 플레이어는 공격, 스킬, 아이템 사용, 방어 등의 행동을 선택하여 전략적으로 턴을 운영해야 합니다.

## 🎯 프로젝트 목표

-   **데이터 기반 설계:** 몬스터, 스킬, 아이템 등 게임의 모든 주요 요소를 `JSON` 파일로 분리하여, 코드 수정 없이 데이터만으로 게임 밸런스와 콘텐츠를 쉽게 확장하고 관리할 수 있는 구조를 목표로 합니다.
-   **객체지향 및 컴포넌트 시스템:** `Actor`, `AbilitySystemComponent`, `InventoryComponent` 등 명확한 역할과 책임을 가진 클래스와 컴포넌트 기반으로 설계하여, 코드의 재사용성과 유지보수성을 높이는 것을 목표로 합니다.
-   **상태 머신 기반 UI:** `enum class`를 활용한 상태 머신을 통해, 일시정지 메뉴, 상점, 인벤토리 등 복잡한 UI의 흐름을 체계적이고 안정적으로 제어하는 것을 목표로 합니다.

## 📂 프로젝트 구조 (Project Structure)
```
Desperado/
├── Data/                 # 게임 데이터 JSON 파일
│   ├── Items.json
│   ├── Map.json
│   ├── Monsters.json
│   └── Skills.json
│
├── Sounds/               # 사운드 리소스
│   ├── Skills/
│   └── UI/
│
├── Source/               # C++ 소스 코드
│   ├── Core/             # 게임의 핵심 기반 클래스 (GameWorld, Actor)
│   ├── Framework/        # 재사용 가능한 시스템 프레임워크 (어빌리티, 인벤토리)
│   │   ├── AbilitySystem/
│   │   └── Inventory/
│   ├── GameLogic/        # 실제 게임 콘텐츠 로직
│   │   ├── Items/
│   │   ├── Map/
│   │   ├── Skills/
│   │   ├── Story/
│   │   └── Units/ (Player, Monster)
│   │
│   ├── BattleManager.h/cpp   # 전투 관리
│   ├── DataManager.h/cpp     # 데이터 관리
│   ├── MainMenu.h/cpp        # 메인 메뉴 UI/로직
│   ├── PauseMenu.h/cpp       # 일시정지 메뉴/상점 UI/로직
│   └── SaveManager.h/cpp     # 저장/불러오기 관리
│
└── Utils/              # 범용 유틸리티 (콘솔 렌더러, 문자열 변환 등)
```

## 🖼️ 콘텐츠 미리보기

| 필드 탐험 화면 | 스탯 분배 |
|:---:|:---:|
| <img src="https://github.com/user-attachments/assets/e595fdf8-a9a0-451c-92d2-e6c7703c945e" width="400"/> | <img src="https://github.com/user-attachments/assets/cb209833-e086-4511-b125-50a9a4876e9b" width="400"/> |

| 전투 화면 | 인벤토리 화면 |
|:---:|:---:|
| <img src="https://github.com/user-attachments/assets/752e7f91-f6a9-4c86-8bc2-a18cb504b151" width="400"/> | <img src="https://github.com/user-attachments/assets/199dda3c-7870-45de-a4ea-fae7647fb49b" width="400"/> |

## ✨ 주요 기능 및 콘텐츠

-   **전략적 턴제 전투:** 공격, 스킬, 아이템, 방어를 활용
-   **어빌리티 시스템:** `GameplayEffect`를 활용한 15종 이상의 다양한 스킬 및 버프/디버프
-   **아이템 & 인벤토리:** 소모품, 장비(무기, 방어구, 장신구)의 획득, 사용, 장착/해제, 버리기(수량 지정), 자동 정렬 기능
-   **상점 시스템:** 아이템 구매(수량 지정) 및 판매(판매가 60% 적용) 기능
-   **저장/불러오기:** 플레이어의 모든 진행 상황(스탯, 스킬, 인벤토리, 위치) 보존
-   **절차적 월드 탐험:** 여러 테마의 맵과 랜덤 인카운터를 통한 월드 이동 기능

## 📝 과제 내용과의 비교 및 차별점

본 프로젝트는 주어진 과제의 요구사항을 기반으로 하되, 보다 완성도 높은 게임 경험을 제공하기 위해 여러 부분을 확장 및 심화하여 구현했습니다.

### 1. 스탯 및 성장 시스템
-   **과제 내용:** HP, 공격력의 단순 스탯 구조. 레벨업 시 고정 수치 증가.
-   **`Desperado` 구현:** **힘, 민첩, 지능, 방어력, 마법 저항력, 치명타** 등 세분화된 2차 스탯을 포함한 `AttributeSet`을 도입했습니다. 힘과 지능 스탯에 따라 최대 HP와 MP가 동적으로 변하는 등, 클래식 RPG의 깊이 있는 성장 시스템을 구현했습니다. 레벨업 시 추가 스탯 포인트를 직접 분배하는 기능도 포함됩니다.

### 2. 전투 시스템
-   **과제 내용:** 자동 턴제 전투.
-   **`Desperado` 구현:** 플레이어가 매 턴 직접 **공격, 스킬, 아이템 사용을 선택**하는 **전략적인 수동 턴제 전투**를 구현하여 게임의 깊이를 더했습니다. 전투 시작 방식 또한, 필드를 자유롭게 탐험하다 몬스터와 조우하는 **'랜덤 인카운터'** 방식을 도입하여 고전 RPG의 감성을 살렸습니다.

### 3. 아이템 및 인벤토리 시스템
-   **과제 내용:** 단순 효과(HP+50, 공격력+10)를 가진 아이템의 랜덤 자동 사용.
-   **`Desperado` 구현:** 데이터 기반(`JSON`)으로 아이템을 정의하고, 장착 가능한 **장비(무기, 방어구, 장신구)**와 **소모품**을 명확히 구분했습니다. 스태킹, 장착/해제, 수량 지정 버리기, 자동 정렬 등 완성도 높은 인벤토리 기능을 플레이어가 직접 제어할 수 있도록 구현했습니다. 과제 요구사항의 공격력 아이템은 세분화된 스탯 시스템에 맞춰 다양한 버프 아이템으로 대체되었습니다.

### 4. 상점 시스템
-   **과제 내용:** 전투 후 Y/N으로 방문.
-   **`Desperado` 구현:** 월드 내에 존재하는 **NPC와 상호작용하여 진입**하는 방식을 채택하여 몰입감을 높였습니다. 아이템 구매/판매 시 수량을 직접 입력할 수 있으며, 과제 요구사항과 동일하게 **판매가는 정가의 60%로 정확히 구현**했습니다.

## 🎮 테스트 (How To Test)

- [**주요 기능 테스트**](https://teamsparta.notion.site/How-To-Test-21e2dc3ef5148084a2a9f0c2a4936a0d?source=copy_link)

## 🛠️ 기술 스택 (Tech Stack)

-   **Programming Language:** C++
-   **IDE & Compiler:** Visual Studio 2022
-   **라이브러리:**
    -   `nlohmann/json`: JSON 데이터 파싱
    -   `Windows Console API`: 화면 렌더링 및 입력 처리
-   **버전 관리:** Git, GitHub

## 👨‍💻 팀원 (Team)

| 이름 | 역할 | GitHub |
| :--- | :--- | :--- |
| `최윤호` | 인벤토리, 상점 시스템, 아이템 사용 | [YouKnowGit](https://github.com/YouKnowGit) |
| `곽규환` | 전투 시스템, 게임 저장, 시스템 설계 | [gwakgh](https://github.com/gwakgh) |
| `김준우` | 사운드, 몬스터/스킬 콘텐츠 | [Bathboomsoso](https://github.com/Bathboomsoso) |
| `김민구` | 맵 구현, 게임 밸런싱 | [1azyox](https://github.com/1azyox) |
| `정현수` | 게임 스토리 | [jhs10507](https://github.com/jhs10507) |
| `김기인` | 아이템 기능 | [kki6700](https://github.com/kki6700) |

## 🔗 관련 링크

-   **[Desperado GitHub Repository](https://github.com/YouKnowGit/Spartale)**
