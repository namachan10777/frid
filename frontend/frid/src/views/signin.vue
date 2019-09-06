<template>
  <div class="signin">
    <section class="logo">
      <img src="../assets/logo.png">
    </section>
    <section class="login">
    <button type="button" @click="signIn">ログイン</button>
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
      if(user){
        this.$router.push('/home')
      }
    });
  }
};
</script>

<style>
.logo{
  display: inline-block;
  width: 300px;
  margin: 0 auto;
}

.login{
  margin: 0 auto; 
}
</style>