<template>
  <div class="signin">
    <section class="logo">
      <img src="../assets/logo.png" />
    </section>
    <section class="login">
      <button class="login-button" @click="signIn">ログイン</button>
    </section>
  </div>
</template>

<script>
import firebase from "firebase";

export default {
  name: "signin",

  data() {
    return {};
  },

  methods: {
    // ログイン
    signIn() {
      const provider = new firebase.auth.GoogleAuthProvider();
      firebase.auth().signInWithPopup(provider);
    }
  },

  mounted: function() {
    firebase.auth().onAuthStateChanged(user => {
      if (user) {
        this.$router.push("/home");
      }
    });
  }
};
</script>

<style>
.logo {
  max-width: 300px;
  margin: 0 auto;
}

.login {
  padding-top: 40px;
  max-width: 140px;
  margin: 0 auto;
}

.login-button {
  padding: 0.4em 3em;
  /* text-decoration: none; */
  background: #009e8c; /* ボタン色 */
  color: #fff;
  border-radius: 8px;
  border-color: white;
}
</style>