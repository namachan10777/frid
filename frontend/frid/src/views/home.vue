<template>
  <div class="home">
    <header>
      <div @click="jump_home">
        <a href="#/home"><img src="../assets/logo-header.png" /></a>
      </div>
      <p class="login-info">
        <i class="fas fa-user"></i>
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
          <i class="fas fa-box" aria-hidden="true"></i>
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
        <i class="fas fa-utensils" aria-hidden="true"></i>
      </div>

      <div class="exp-date">
        <el-date-picker
          v-model="exp_date"
          size="large"
          value-format="yyyy/MM/dd"
          type="date"
          placeholder="消費期限"
        ></el-date-picker>
      </div>
      <div class="add-button">
        <el-button @click="addFoods" type="primary" icon="el-icon-check" round></el-button>
      </div>
    </div>

    <div v-if="hide_add">
      <transition-group name="el-fade-linear">
        <li
          class="foods-list"
          v-for="item in list"
          :key="item.tapper_id"
          v-show="item.is_active && item.temperature >= 0"
        >
          <p>タッパーID:{{ item.tapper_id }}</p>
          <p>食材名:{{ item.name }}</p>
          <p>温度:{{ item.temperature }}</p>
          <p>消費期限:{{ item.exp_date }}</p>
          <input
            type="image"
            class="delete-button"
            alt="delete"
            @click="deleteFoods(item.tapper_id)"
            src="https://i.imgur.com/sntj3EF.png"
            width="30"
            height="30"
          />
        </li>
      </transition-group>
    </div>

    <div v-if="hide_add">
      <a href="#/home" @click="page_add" class="page-add">+</a>
    </div>
    <div class="qr" v-if="!hide_qr">
      <qrcode-stream @decode="onDecode" @init="onInit" />
      {{ error }}
      {{ result }}
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
      hide_add: true
    };
  },

  methods: {
    jump_home() {
      this.hide_qr = true;
      this.hide_add = true;
    },
    page_add() {
      // const url = "https://hooks.slack.com/services/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";
      // const data = {
      //   text: String(this.hide_qr)
      // };
      // axios.post(url, JSON.stringify(data), {
      //   withCredentials: false,
      //   transformRequest: [
      //     (data, headers) => {
      //       delete headers.post["Content-Type"];
      //       return data;
      //     }
      //   ]
      // });
      // axios.post('https://hooks.slack.com/services/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', {text: "hoge"})
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
      } else {
        alert("入力に不備があります．");
      }

      this.tapper_id = null;
      this.exp_date = "";
      this.food_name = "";
    },

    // is_active -> falseにしてリスト非表示
    deleteFoods(id) {
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
    },
  },

  created: function() {
    const user = firebase.auth().currentUser;

    if (user) {
      this.user = user;
    } else {
      this.$router.push("/signin");
    }

    // firestore からデータを取得
    firebase
      .firestore()
      .collection("foods")
      .onSnapshot(ss => {
        let buff = [];
        ss.forEach(doc => buff.push(doc.data()));
        this.list = buff;
      });
  }
};
</script>

<style scoped>
.main {
  padding: 30px 20px 60px 35px;
  border-radius: 15px;
  max-width: 300px;
  margin: 0 auto;
  box-shadow: 0 2px 12px 0 rgba(0, 0, 0, 0.1);
}
.foods-list {
  font-weight: bold;
  border-radius: 15px;
  width: 300px;
  text-align: center;
  background-color: #53d6c7;
  margin: 20px;
  display: inline-block;
}

.input-form {
  position: relative;
  /* width: 10%; */
  /* margin-left: 20px; */
}

.qr {
  max-width: 200px;
  margin: 0 auto;
}

.page-add {
  position: absolute;
  right: 20px;
  bottom: 20px;
  width: 10%;
  margin-left: 20px;
  font-size: 30px;
  display: inline-block;
  text-decoration: none;
  background: #009e8c;
  color: #fff;
  width: 60px;
  height: 60px;
  line-height: 60px;
  border-radius: 50%;
  text-align: center;
  font-weight: bold;
  overflow: hidden;
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

.exp-date {
  margin-top: 20px;
}

.qr-reader {
  padding-left: 10px;
  padding-top: 13px;
  width: 25px;
  height: 25px;
}

.add-button {
  float: right;
  margin-top: 10px;
  margin-right: 5px;
}

header {
  width: 100%;
  height: 70px;
  display: inline-flex;
  background: #009e8c;
}

h1 {
  font-size: 50px;
  padding-left: 20px;
  margin-top: 0;
}

h1::first-letter {
  color: white;
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
.logout-button:hover{
  opacity: 0.5;
}
</style>