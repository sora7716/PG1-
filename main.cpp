#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

const char kWindowTitle[] = "イイヅカ_ソラ";

//定数
const int kWindowWidth = 1280; //ウィンドウの幅
const int kWindowHeight = 720; //ウィンドウの高さ

//二次元のベクトル
typedef struct Vector2 {
	float x;
	float y;
}Vector2;

//ゲームオブジェクト
typedef struct GameObject {
	Vector2 position;//位置
	Vector2 speed;//速度
	float radius;//半径
	unsigned int color;//色
	int isAlive; //生存フラグ
}GameObject;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//プレイヤー
	GameObject player{};
	player.position = {
		static_cast<float>(kWindowWidth / 2),
		static_cast<float>(kWindowHeight - 100.0f)
	};//位置
	player.speed = { 10.0f,10.0f };//速度
	player.radius = 50.0f;//サイズ
	player.color = WHITE;//色
	player.isAlive = true;//生存フラグ
	//移動方向
	Vector2 playerDirection = {};

	//弾
	GameObject bullet{};
	bullet.position = {};//位置
	bullet.speed = { 0.0f,10.0f };//速度
	bullet.radius = 50.0f;//サイズ
	bullet.color = RED;//色
	bullet.isAlive = false;//生存フラグ

	//敵
	GameObject enemy{};
	enemy.position = { 0.0f,100.0f };//位置
	enemy.speed = { 0.0f,0.0f };//速度
	enemy.radius = 30.0f;//サイズ
	enemy.color = BLUE;//色
	enemy.isAlive = true;//生存フラグ
	float enemyForTheta = 0.0f;//敵のθ
	float enemyForAmplitude = static_cast<float>(kWindowWidth / 2) - enemy.radius;//敵の振幅
	float enemyRespawnTimer = 0.0f;//敵のリスポーンタイマー

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		///
		///↓プレイヤー
		///
		//上下
		if (keys[DIK_W]) {
			playerDirection.y = -1.0f;
		} else if (keys[DIK_S]) {
			playerDirection.y = 1.0f;
		} else {
			playerDirection.y = 0.0f;
		}

		//左右
		if (keys[DIK_A]) {
			playerDirection.x = -1.0f;
		} else if (keys[DIK_D]) {
			playerDirection.x = 1.0f;
		} else {
			playerDirection.x = 0.0f;
		}

		//移動ベクトルの長さを計算
		float len = sqrtf(powf(playerDirection.x, 2.0f) + powf(playerDirection.y, 2.0f));
		if (len != 0.0f) {
			//正規化
			playerDirection.x /= len;
			playerDirection.y /= len;
		}

		//プレイヤーの移動
		player.position.x += playerDirection.x * player.speed.x;
		player.position.y += playerDirection.y * player.speed.y;
		///
		///↑プレイヤー
		///

		///
		///↓弾
		///
		//発射
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
			//弾が存在していたら
			if (!bullet.isAlive) {
				bullet.isAlive = true;//弾の生存フラグを立てる
				bullet.position = player.position;//プレイヤーの位置から発射
			}
		}

		//移動(存在していたら)
		if (bullet.isAlive) {
			//画面外へ行ったら
			if (bullet.position.y < 0.0f) {
				bullet.isAlive = false;//生存フラグを折る
			} else {
				//上の方向へ
				bullet.position.y -= bullet.speed.y;
			}
		}
		///
		///↑弾
		///

		///
		///↓敵
		///
		//移動(存在していたら)
		if (enemy.isAlive) {
			//サイン波で動くように
			enemyForTheta += static_cast<float>(M_PI) / 120.0f;
			enemy.position.x = sinf(enemyForTheta) * enemyForAmplitude + static_cast<float>(kWindowWidth / 2);

		} else {
			//存在していなかったらリスポーンタイマーをカウントダウン
			if (enemyRespawnTimer > 0.0f) {
				enemyRespawnTimer--;
			} else {
				enemy.isAlive = true; //生存フラグを立てる

			}
		}
		///
		///↑敵
		///

		///
		///↓衝突判定
		///
		//敵と弾が存在しているなら
		if (enemy.isAlive && bullet.isAlive) {
			//敵と弾の距離を計算
			Vector2  enemyToBulletDistance = { enemy.position.x - bullet.position.x,enemy.position.y - bullet.position.y };
			//敵と弾の距離の長さを計算
			float enemyToBulletLen = sqrtf(powf(enemyToBulletDistance.x, 2.0f) + powf(enemyToBulletDistance.y, 2.0f));

			//衝突していたら
			if (enemyToBulletLen < enemy.radius + bullet.radius) {
				enemy.isAlive = false; //敵の生存フラグを折る
				bullet.isAlive = false; //弾の生存フラグを折る
				enemyRespawnTimer = 120.0f; //リスポーンタイマーをセット
			}
		}
		///
		///↑衝突判定
		///

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		///
		///プレイヤー
		///
		if (player.isAlive) {
			Novice::DrawBox(
				static_cast<int>(player.position.x - player.radius),
				static_cast<int>(player.position.y - player.radius),
				static_cast<int>(player.radius * 2.0f),
				static_cast<int>(player.radius * 2.0f),
				0.0f,
				player.color,
				kFillModeSolid
			);
		}

		///
		///弾
		///
		if (bullet.isAlive) {
			Novice::DrawTriangle(
				static_cast<int>(bullet.position.x - bullet.radius / 2.0f),
				static_cast<int>(bullet.position.y + bullet.radius / 2.0f),
				static_cast<int>(bullet.position.x),
				static_cast<int>(bullet.position.y - bullet.radius / 2.0f),
				static_cast<int>(bullet.position.x + bullet.radius / 2.0f),
				static_cast<int>(bullet.position.y + bullet.radius / 2.0f),
				bullet.color,
				kFillModeSolid
			);
		}

		///
		///敵
		///
		if (enemy.isAlive) {
			Novice::DrawEllipse(
				static_cast<int>(enemy.position.x),
				static_cast<int>(enemy.position.y),
				static_cast<int>(enemy.radius),
				static_cast<int>(enemy.radius),
				0.0f,
				enemy.color,
				kFillModeSolid
			);
		}

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
