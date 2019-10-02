<template>
  <div class="home">
    <header>
      <div @click="jump_home">
        <a href="#/home">
          <img src="../assets/logo-header.png" />
        </a>
      </div>
      <p class="login-info">
        <i class="el-icon-user"></i>
        {{ user.displayName }}
      </p>
      <div>
        <button class="logout-button" @click="signOut">ログアウト</button>
      </div>
    </header>

    <div class="main" v-if="hide_qr && !hide_add">
      <div class="input-form">
        <div class="tapper">
          <input type="text" v-model="tapper_id" placeholder="タッパーID" />
          <i class="el-icon-box"></i>
          <div class="qr-reader">
            <input
              type="image"
              alt="qr"
              src="https://dab1nmslvvntp.cloudfront.net/wp-content/uploads/2017/07/1499401426qr_icon.svg"
              @click="qr_page"
              width="25"
              height="25"
            />
          </div>
        </div>
      </div>

      <div class="input-form">
        <input type="text" v-model="food_name" placeholder="食材名" />
        <i class="el-icon-knife-fork"></i>
      </div>

      <div class="exp-date">
        <el-date-picker
          v-model="exp_date"
          size="large"
          value-format="yyyy-MM-dd"
          type="date"
          placeholder="消費期限"
        ></el-date-picker>
      </div>
      <div class="add-button">
        <el-button @click="cancel_add" type="danger" icon="el-icon-close" round></el-button>
        <el-button @click="addFoods" type="primary" icon="el-icon-check" round></el-button>
      </div>
    </div>

    <div v-if="hide_add">
      <li
        class="foods-list"
        v-for="item in list"
        :key="item.tapper_id"
        v-show="item.is_active && item.temperature >= 0"
      >
        <el-card
          class="card"
          v-bind:class="{ exp_warn: food_warn[0], temp_warn: food_warn[1] }"
          shadow="hover"
        >
          <div slot="header" class="clearfix">
            <span>{{ item.name }}</span>
            <el-button
              style="float: right; padding: 3px 0"
              type="text"
              @click="deleteFoods(item.tapper_id)"
            >Delete</el-button>
          </div>
          <p class="temp">温度: {{ item.temperature }} ℃</p>
          <p>消費期限: {{ item.exp_date }}</p>
        </el-card>
      </li>
    </div>

    <transition name="el-fade-in">
      <div v-if="hide_add">
        <a href="#/home" @click="page_add" class="page-add">+</a>
      </div>
    </transition>
    <div class="qr" v-if="!hide_qr">
      <qrcode-stream @decode="onDecode" @init="onInit" />
      {{ error }}
      {{ result }}
      <el-button class="qr-cancel" @click="qr_cancel" type="info" plain>Cancel</el-button>
    </div>
  </div>
</template>


<script>
import firebase from "firebase";
import { QrcodeStream } from "vue-qrcode-reader";
import axios from "axios";
import moment from "moment";

export default {
  name: "home",
  components: {
    QrcodeStream
  },
  data() {
    return {
      user: {}, // firebase ユーザ情報
      exp_date: "",
      tapper_id: null,
      list: [],
      food_name: "",
      result: "",
      error: "",
      hide_qr: true,
      hide_add: true,
      temp_counter: 0,
      is_exp: false,
      food_warn: [false, false, false],
      flag: false
    };
  },
  methods: {
    post_to_slack(message) {
      const url =
        "https://hooks.slack.com/services/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";
      const data = {
        text: message
      };
      axios.post(url, JSON.stringify(data), {
        withCredentials: false,
        transformRequest: [
          (data, headers) => {
            delete headers.post["Content-Type"];
            return data;
          }
        ]
      });
    },
    jump_home() {
      this.hide_qr = true;
      this.hide_add = true;
    },
    page_add() {
      this.hide_add = false;
    },
    onDecode(result) {
      this.tapper_id = result;
      this.hide_qr = true;
    },

    async onInit(promise) {
      try {
        await promise;
      } catch (error) {
        if (error.name === "NotAllowedError") {
          this.error = "You need to grant camera access permisson";
        } else if (error.name === "NotFoundError") {
          this.error = "No camera on this device";
        } else if (error.name === "NotSupportedError") {
          this.error = "Secure context required (HTTPS, localhost)";
        } else if (error.name === "NotReadableError") {
          this.error = "Is the camera already in use?";
        } else if (error.name === "OverconstrainedError") {
          this.error = "Installed cameras are not suitable";
        } else if (error.name === "StreamApiNotSupportedError") {
          this.error = "Stream API is not supported in this browser";
        }
      }
    },

    // QR読取り
    qr_page() {
      this.hide_qr = false;
    },

    // QR読取りキャンセル
    qr_cancel() {
      this.hide_qr = true;
    },
    // ログアウト
    signOut() {
      firebase.auth().signOut();
      this.$router.push("/signin");
    },

    // firestore cloud に登録
    addFoods() {
      if (
        this.tapper_id != null &&
        this.exp_date != "" &&
        this.food_name != ""
      ) {
        firebase
          .firestore()
          .collection("foods")
          .doc(String(this.tapper_id))
          .set(
            {
              tapper_id: this.tapper_id,
              exp_date: this.exp_date,
              is_active: true,
              name: this.food_name
            },
            { merge: true }
          );
        this.hide_add = true;
        if (this.list[this.tapper_id].name != "") {
          this.$message({
            message: "食材情報が上書きされました．",
            type: "success"
          });
        } else {
          this.$message({
            message: "食材が追加されました．",
            type: "success"
          });
        }

        this.tapper_id = null;
        this.exp_date = "";
        this.food_name = "";
      } else {
        this.$message.error("入力に不備があります．");
      }
    },

    cancel_add() {
      this.$message("入力がキャンセルされました．");

      this.hide_add = true;

      this.tapper_id = null;
      this.exp_date = "";
      this.food_name = "";
    },
    // is_active -> falseにしてリスト非表示
    deleteFoods(id) {
      this.$confirm("この食材を削除しますか？", "Warning", {
        confirmButtonText: "OK",
        cancelButtonText: "Cancel",
        type: "warning"
      })
        .then(() => {
          firebase
            .firestore()
            .collection("foods")
            .doc(String(id))
            .set(
              {
                exp_date: "",
                name: "",
                is_active: false
              },
              { merge: true }
            );
          this.food_warn[0] = false;
          this.$message({
            type: "success",
            message: "Delete completed"
          });
        })
        .catch(() => {
          this.$message({
            type: "info",
            message: "Delete canceled"
          });
        });
    }
  },

  created() {
    var user = firebase.auth().currentUser;

    if (user) {
      this.user = user;
    }

    // firestore からデータを取得
    firebase
      .firestore()
      .collection("foods")
      .onSnapshot(ss => {
        let buff = [];
        ss.forEach(doc => buff.push(doc.data()));
        this.list = buff;

        // 温度管理
        if (this.list[0].is_active) {
          if (this.list[0].temperature > 18) {
            this.temp_counter++;
          } else {
            this.temp_counter = 0;
            this.food_warn[1] = false;
          }

          if (this.temp_counter >= 5) {
            this.flag = true;
            this.temp_counter = 0;
          }

          if (this.flag) {
            var temp_notify =
              String(this.list[0].name) + " が冷蔵庫から出しっぱなしです．";
            this.post_to_slack(temp_notify);
            this.flag = false;
            this.food_warn[1] = true;
          }

          var now = moment();
          var exp = moment(this.list[0].exp_date);

          var tmp = false;
          if (exp.diff(now, "days") + 1 <= 2) {
            tmp = true;
          }

          if (!this.is_exp && tmp) {
            var exp_notify =
              String(this.list[0].name) +
              " の消費期限が近づいています．\n消費期限は " +
              String(this.list[0].exp_date) +
              " です．";
            this.post_to_slack(exp_notify);
            this.is_exp = true;
            this.food_warn[0] = true;
          }
        }
      });
  }
};
</script>

<style scoped>
/* 食材追加画面 */
.main {
  padding: 30px 20px 60px 35px;
  border-radius: 15px;
  max-width: 300px;
  margin: 0 auto;
  box-shadow: 0 2px 12px 0 rgba(0, 0, 0, 0.1);
}

/* 食材リストのデザイン */
li {
  list-style: none;
}
.card {
  margin: 0.8vw;
  border-radius: 20px;
}
.card p {
  margin: 1vh;
}

/* 食材リストの色を変更 */
.exp_warn {
  background-color: #f5cd60;
}
.temp_warn {
  background-color: #f56a60;
}

/* タッパーID，食材名入力フォーム */
.input-form {
  position: relative;
}
input {
  max-width: 220px;
}
.input-form input[type="text"] {
  font: 15px/24px sans-serif;
  box-sizing: border-box;
  margin: 8px 0;
  padding: 0.3em;
  transition: 0.3s;
  border: 1px solid #1b2538;
  border-radius: 4px;
  outline: none;
}
.input-form input[type="text"]:focus {
  border-color: #da3c41;
}
.input-form input[type="text"] {
  padding-left: 40px;
}
.input-form i {
  position: absolute;
  top: 8px;
  left: 0;
  padding: 9px 8px;
  transition: 0.3s;
  color: #aaaaaa;
}
.input-form input[type="text"]:focus + i {
  color: #da3c41;
}
.input-form .tapper {
  max-width: 190px;
  display: inline-flex;
}
/* QRコードアイコン */
.qr-reader {
  padding-left: 10px;
  padding-top: 13px;
  width: 25px;
  height: 25px;
}

/* QR読み込み画面 */
.qr {
  width: 320px;
  margin: 0 auto;
}
.qr-cancel {
  margin-top: 0.5vh;
}

/* 食材追加ボタン ⊕ */
.page-add {
  position: absolute;
  right: 20px;
  bottom: 20px;
  margin-left: 20px;
  font-size: 30px;
  text-decoration: none;
  background: #009e8c;
  color: #fff;
  width: 60px;
  height: 60px;
  line-height: 60px;
  border-radius: 50%;
  text-align: center;
  font-weight: bold;
  box-shadow: 0px 2px 2px rgba(0, 0, 0, 0.29);
  border-bottom: solid 3px #009e8c;
  transition: 0.4s;
}
.page-add:active {
  -webkit-transform: translateY(2px);
  transform: translateY(2px);
  box-shadow: 0 0 1px rgba(0, 0, 0, 0.15);
  border-bottom: none;
}

/* 消費期限のDatePicker */
.exp-date {
  margin-top: 10px;
}

/* 食材追加確定ボタン */
.add-button {
  float: right;
  margin-top: 10px;
  margin-right: 5px;
}
.add-button:focus-visible {
  outline: none;
}

/* ヘッダー */
header {
  width: 100%;
  height: 70px;
  display: inline-flex;
  background: #009e8c;
}
.login-info {
  font-family: "Segoe UI", Tahoma, Geneva, Verdana, sans-serif;
  font-size: 20px;
  margin: 0 0 0 auto;
  margin-top: 20px;
  margin-right: 10px;
  text-align: right;
  color: white;
}
.logout-button {
  font-family: "Segoe UI", Tahoma, Geneva, Verdana, sans-serif;
  height: 70px;
  margin-right: 10px;
  margin-left: 10px;
  background: #009e8c; /* ボタン色 */
  border: #009e8c;
  color: white;
  cursor: pointer;
  transition: all 0.3s ease 0s;
}
.logout-button:hover {
  opacity: 0.5;
}
</style>